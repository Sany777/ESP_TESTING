#include "event_manager.h"
#include "event_manager_internal.h"

#include "device.h"


#define MAX_EVENT_HANDLERS 100

QueueHandle_t evn_queue = NULL;
static EventHandlerRegistry event_handler_registry[MAX_EVENT_HANDLERS];


#define NO_HANDLER NULL
#define ANY_HANDLER NULL


void reset_event_list()
{
	for(int i=0;i<MAX_EVENT_HANDLERS;i++){
		event_handler_registry[i].event_id = EVN_NO_ENTRY;
		event_handler_registry[i].handler = NO_HANDLER;
	}
}

static
EventHandlerRegistry * get_event_entry(int *counter, EventId event_id, EventHandler handler)
{
	EventHandlerRegistry * entry = NULL;
	int i;
	if(counter) i = *counter;
	else i = 0;
	for (; i < MAX_EVENT_HANDLERS; i++) {
		if ((event_id == EVN_ANY || event_handler_registry[i].event_id == event_id) 
				&& (handler == ANY_HANDLER || event_handler_registry[i].handler == handler)){
			entry = &event_handler_registry[i];
			break;
		}
	}
	if(counter) *counter = i+1;
	return entry;
}


void unregister_event_handler(EventHandler handler) 
{
	if(handler == ANY_HANDLER) return;
    EventHandlerRegistry * entry;
	int i = 0;
	while((entry = get_event_entry(&i, EVN_ANY, handler)) != NULL){
		entry->event_id = EVN_NO_ENTRY;
		entry->handler = NO_HANDLER;
	}
}

void unregister_event_id(EventId event_id) 
{
    EventHandlerRegistry * entry;
	if(event_id == EVN_NO_ENTRY) return;
	int i = 0;
	while((entry = get_event_entry(&i, event_id, ANY_HANDLER)) != NULL){
		entry->event_id = EVN_NO_ENTRY;
		entry->handler = NO_HANDLER;
	}
}

bool register_event_handler(EventId event_id, void (*handler)(unsigned now_ms, void *ctx)) 
{
	if(handler == NULL) return false;
	if(get_event_entry(NULL, event_id, handler) != NULL){
		ESP_LOGE("Warning", "The handler was registered!");
		return true; 
	}
	EventHandlerRegistry * empty_space = get_event_entry(NULL, EVN_NO_ENTRY, ANY_HANDLER);
	if(empty_space != NULL){
		empty_space->event_id = event_id;
		empty_space->handler = handler;
		return true;
	}
	return false;
}

static
void event_manager_task(void*)
{
	unsigned ms;
	DevEvn event;
	for(;;){
		vTaskDelay(pdMS_TO_TICKS(10));
        if (xQueueReceive(evn_queue, &event, portMAX_DELAY) == pdPASS){
			EventHandlerRegistry *entry = NULL;
			ms = ms_counter;
			int i = 0;
			while((entry = get_event_entry(&i, event.event_id, ANY_HANDLER)) != NULL){
				if(entry->handler != NULL) entry->handler(ms, event.ctx);
			}
        }
	}
}

bool set_event(int event_id, void *ctx)
{
	DevEvn event;
	event.event_id = event_id;
	event.ctx = ctx;
	return xQueueSendToBack(evn_queue, &event, 100) == pdTRUE;
}

static StaticTask_t task_buffer; 
static StackType_t task_stack[STACK_SIZE];
static uint8_t evn_queue_storage[EVN_QUEUE_LEN * sizeof(DevEvn)]; 
static StaticQueue_t evn_queue_buffer;

void init_event_manager()
{
	if(evn_queue != NULL) return;
	reset_event_list();
    evn_queue = xQueueCreateStatic(EVN_QUEUE_LEN, sizeof(DevEvn), evn_queue_storage, &evn_queue_buffer);
    assert(evn_queue != NULL);
	init_event_time_manager();
	xTaskCreateStatic(
		event_manager_task,  
		"evn_manager_task",   
		STACK_SIZE,     
		NULL, 
		1,
		task_stack,
		&task_buffer
	);
}
