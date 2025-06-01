#include "event_manager.h"
#include "event_manager_internal.h"

#include "esp_timer.h"


volatile unsigned ms_counter;
static QueueHandle_t periodic_evn_queue = NULL;
static PeriodicEvent periodic_evn_list[PERIODIC_EVN_LIST_LEN];
static void push_periodic_event(PeriodicEvent *new_event);

static 
void IRAM_ATTR periodic_event_timer_handler(void *arg)
{
    BaseType_t task_woken = pdFALSE;
    PeriodicEvent new_event;
    while (xQueueReceiveFromISR(periodic_evn_queue, &new_event, NULL) == pdTRUE){
		push_periodic_event(&new_event);
	}
    for (PeriodicEvent *event = periodic_evn_list;
			event < periodic_evn_list + PERIODIC_EVN_LIST_LEN;
			++event){
        if (event->invoke == NO_CALL)
            continue;
        if (event->delay > 0) event->delay--;
        if (event->delay == 0){
            if (event->invoke > 0) event->invoke--;
            if (event->invoke != NO_CALL) event->delay = event->init_delay;
            xQueueSendFromISR(evn_queue, &event->evn, &task_woken);
        }
    }
    portYIELD_FROM_ISR(task_woken);
	ms_counter++;
}


void stop_periodic_event(int event_id)
{
	start_periodic_event(event_id, NULL, 0, NO_CALL);
}

bool start_periodic_event(int event_id, void *ctx, uint64_t delay,  int32_t invoke)
{
	PeriodicEvent per_event = {
		.evn.event_id = event_id, 
		.evn.ctx = ctx, 
		.delay = delay, 
		.init_delay = delay, 
		.invoke = invoke
	};
	return xQueueSendFromISR(periodic_evn_queue, &per_event, NULL) == pdTRUE;
}

static
void push_periodic_event(PeriodicEvent *new_event)
{
	PeriodicEvent *evn = periodic_evn_list, 
				*list_end = periodic_evn_list + PERIODIC_EVN_LIST_LEN;
	while(evn < list_end && evn->evn.event_id != new_event->evn.event_id){ evn++; }
	if(evn >= list_end){
		evn = periodic_evn_list;
		while(evn < list_end){
			if(evn->invoke == NO_CALL){
				evn->evn.event_id = new_event->evn.event_id;
				break;
			}
			evn++;
		}
	}
	if(evn < list_end){
		evn->init_delay = evn->delay = new_event->init_delay;
		evn->invoke = new_event->invoke;
		evn->evn.ctx = new_event->evn.ctx;
	}
#ifdef DEBUG
else {
	if(!res) ESP_LOGE("","%s", " full task list!!");
}	
#endif
}

static uint8_t periodic_evn_queue_storage[NEW_EVN_QUEUE_LEN * sizeof(PeriodicEvent)]; 
static StaticQueue_t periodic_evn_queue_buffer; 
static esp_timer_handle_t timer_handle = NULL;

void init_event_time_manager()
{
	if(periodic_evn_queue != NULL) return;
	ms_counter = 0;
	const esp_timer_create_args_t timer_args = {
		.callback = &periodic_event_timer_handler, 
        .arg = NULL,                    
        .dispatch_method = ESP_TIMER_TASK,
        .name = "timer_handler"
    };
	periodic_evn_queue = xQueueCreateStatic(NEW_EVN_QUEUE_LEN, 
						sizeof(PeriodicEvent), 
						periodic_evn_queue_storage, 
						&periodic_evn_queue_buffer);
	assert(periodic_evn_queue != NULL);
	ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 1000));
}