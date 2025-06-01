const barConfigs = [ 
  { field: "humidity_cur", icon: "💧", label: "Humidity" },
  { field: "motion_cur", icon: "🚶", label: "Motion" },
  { field: "lt1", icon: "⬜", label: "LT1" },
  { field: "lt2", icon: "⬜", label: "LT2" },
  { field: "fan_speed", icon: "🌀", label: "Fan" },
];

const formConfigs = [
  {
    legend: "Config Update",
    name: "config",
    class: "hide",
    id: "admin",
    noUpdate: true,
    fields: [
      { label: "Choose .csv file", name: "firmware", type: "file", accept: ".csv" }
    ]
  },
  {
    legend: "Firmware Update",
    name: "update",
    noUpdate: true,
    fields: [
      { label: "Choose .bin file", name: "firmware", type: "file", accept: ".bin" }
    ]
  },
  {
    legend: "Fan Speed",
    name: "speed",
    fields: [
      { label: "Speed I, m³/h", name: "speed1", type: "select", options: ["0","8","10","15","20","30","40","60"] },
      { label: "Speed II, m³/h", name: "speed2", type: "select", options: ["8","10","15","20","30","40","60","100"] },
    ]
  },
  {
    legend: "On/Off Timers",
    name: "timers",
    fields: [
      { label: "Turn-on delay, min", name: "on_timer", type: "select", options: ["0", "1", "2", "5"] },
      { label: "Turn-off delay, min", name: "off_timer", type: "select", options: ["0", "5", "15", "30"] }
    ]
  },
  {
    legend: "Humidity Control",
    name: "humidity",
    fields: [
      { label: "Humidity", name: "humidity", type: "select", options: ["Off", "Auto", "40%", "50%", "60%", "70%", "80%", "90%"] },
    ]
  },
  {
    legend: "Interval Timer",
    name: "interval",
    fields: [
      { label: "Interval", name: "interval_timer", type: "select", options: ["Off", "10m", "30m", "1h"] }
    ]
  },
  {
    legend: "Motion Control",
    name: "motion",
    fields: [
      { label: "Activate", name: "motion_en", type: "checkbox" },
    ]
  },
  {
    legend: "WiFi Control",
    name: "wifi",
    fields: [
      { label: "Activate", name: "wifi_en", type: "checkbox" },
    ]
  },
  {
    legend: "Filter Time",
    name: "filter_reset",
    fields: [],
    noUpdate: true,
    butLabel: "Reset",
  },
];

const modal = document.getElementById("modal");

function el(tag, props = {}, ...children) {
  const elem = document.createElement(tag);
  Object.entries(props).forEach(([key, val]) => {
    if (key in elem) elem[key] = val;
    else elem.setAttribute(key, val);
  });
  children.flat().forEach(child => {
    if (child == null) return;
    if (typeof child === "string") {
      elem.appendChild(document.createTextNode(child));
    } else if (child instanceof Node) {
      elem.appendChild(child);
    }
  });
  return elem;
}

function showModal(type, text) {
  if(!text) return;
  modal.className = `modal ${type}`;
  modal.textContent = text;
  setTimeout(() => {
    modal.classList.add("hidden");
  }, 3000);
}

async function sendFormData(form_name, data = {}) {
  try {
    const hasData = Object.keys(data).length > 0;
    const options = hasData
      ? {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(data),
        }
      : { method: "GET" };

    const res = await fetch("\\" + form_name, options);
    const text = await res.text();

    if (res.ok && text.trim()) {
      showModal("success", text);
    } else if (!res.ok) {
      showModal("error", text || `Error: ${res.status} ${res.statusText}`);
    } else {
      showModal("error", "No response");
    }
  } catch (err) {
    showModal("error", "No response");
    console.error(err);
  }
}

async function loadFormData(form) {
  if (!form) return;
  try {
    const url = new URL(form.name, window.location.origin);
    const res = await fetch(url.href, { method: "GET" });
    if (!res.ok) throw new Error(`${res.status} ${res.statusText}`);
    const json = await res.json();

    if (typeof json === "object" && json !== null) {
      Object.entries(json).forEach(([key, value]) => {
        const input = form.elements.namedItem(key);
        if (!input) return;
        if (input.type === "checkbox") {
          input.checked = Boolean(value);
        } else if (input.tagName === "SELECT") {
          const idx = Number(value);
          if (!isNaN(idx) && input.options[idx]) {
            input.selectedIndex = idx;
          }
        } else if (input.type !== "file") {
          input.value = value;
        }
      });
    }
  } catch (err) {
    console.error("error:", err);
    showModal("error", "Connection error");
  }
}

function addSubmitHandler(form, cfg) {
  form.addEventListener("submit", function(e) {
    e.preventDefault();

    (async function() {
      var hasFileField = false;
      for (var i = 0; i < cfg.fields.length; i++) {
        if (cfg.fields[i].type === "file") {
          hasFileField = true;
          break;
        }
      }

      if (hasFileField) {
        var formData = new FormData(form);
        var fileFieldName = null;
        for (var j = 0; j < cfg.fields.length; j++) {
          if (cfg.fields[j].type === "file") {
            fileFieldName = cfg.fields[j].name;
            break;
          }
        }
        var file = formData.get(fileFieldName);

        if (!file) {
          showModal("error", "No file selected");
          return;
        }

        try {
          var res = await fetch(cfg.name, {
            method: "POST",
            body: formData
          });
          var text = await res.text();
          if (res.ok && text.trim()) {
            showModal("success", text);
          } else {
            showModal("error", text || ("Error: " + res.status + " " + res.statusText));
          }
        } catch (err) {
          showModal("error", "Send failed");
          console.error(err);
        }
        return;
      }

      if (cfg.noUpdate) {
        await sendFormData(cfg.name, {});
        return;
      }

      var data = {};
      for (var k = 0; k < cfg.fields.length; k++) {
        var field = cfg.fields[k];
        var input = form.elements.namedItem(field.name);
        if (!input) continue;

        if (input.type === "checkbox") {
          data[field.name] = input.checked;
        } else if (input.tagName === "SELECT") {
          data[field.name] = input.selectedIndex;
        } else {
          data[field.name] = input.value;
        }
      }

      await sendFormData(cfg.name, data);
    })();
  });
}


function renderInputField(field) {
 let input;

 if (field.type === "select") {
  input = el("select", { name: field.name },
   ...(field.options || []).map(opt => el("option", { value: opt }, opt))
  );
 } else if (field.type === "checkbox") {
  input = el("input", { type: "checkbox", name: field.name });
 } else if (field.type === "file") {
  input = el("input", { type: "file", name: field.name, accept: field.accept });
 } else {
  input = el("input", { type: field.type || "text", name: field.name });
 }

 return el("label", { className: "form-field" },
  field.label,
  input
 );
}

function renderForm(cfg) {
 const { fields, legend, butLabel, noUpdate,...formAttrs } = cfg;
 const form = el("form", formAttrs);
 const fieldset = el("fieldset");
 fieldset.appendChild(el("legend", {}, legend));

 fields.forEach(field => {
  const fieldEl = renderInputField(field);
  fieldset.appendChild(fieldEl);
 });
 form.appendChild(fieldset);
 const butText = butLabel ? butLabel : "Submit";
 form.appendChild(el("button", { type: "submit" }, butText));
 addSubmitHandler(form, cfg);

 return form;
}

function createForms(cfgs) {
 const container = document.getElementById("form-root");
 cfgs.forEach(cfg => {
  const form = renderForm(cfg);
  container.appendChild(form);
 });
}

async function loadData() {
  formConfigs.forEach(cfg => {
    const { name, noUpdate } = cfg;
    if (!noUpdate) {
      const form = document.forms[name];
      loadFormData(form);
    }
  });
}

async function exit() {
 sendFormData("/exit");
}

const bar = document.getElementById('bar');
const infoBar = document.getElementById('info-bar');


async function updateLiveData(url = '/status') {

  try {
    const res = await fetch(url, { cache: 'no-store' });
    const data = res.ok ? await res.json() : {};
    updateBarItems(data);
    updateInfoBar(data.info);
    updateAdmin(data?.admin);
    updateTitle(data?.title);
  } catch (err) {
  } 
}


function updateBarItems(data) {
 bar.querySelectorAll('.bar-item[data-field]').forEach(item => {
  const field = item.dataset.field;
  const strong = item.querySelector('strong');
  if (!strong || !(field in data)) return;

  let val = formatValue(field, data[field]);
  strong.textContent = val;
 });
}

function formatValue(field, value) {
 if (typeof value === 'boolean') return value ? 'Yes' : 'No';
 if (['humidity', 'fan_speed'].includes(field)) return `${value}%`;
 return value;
}

function updateInfoBar(info) {
 if (infoBar) infoBar.textContent = info || '';
}

function updateAdmin(isAdmin) {
 const adminEl = document.getElementById('admin');
 if (isAdmin && adminEl) adminEl.classList.remove('hide');
}

function updateTitle(title) {
 const titleEl = document.getElementById('title');
 if(title && titleEl){
  titleEl.textContent = title;
 }
}

function createBar(cfgs) {
 const container = document.getElementById("bar");
 cfgs.forEach(({ field, icon, label }) => {
  const strong = el("strong", {}, "--");
  const spanText = el("span", {}, `${label}: `, strong);
  const spanIcon = el("span", { className: "icon" }, icon);
  const item = el("div", {
          className: "bar-item",
          "data-field": field
         }, spanIcon, spanText);
  container.appendChild(item);
 });
}

window.onload = () => {
  createForms(formConfigs);
  createBar(barConfigs);
  loadData();
  setInterval(updateLiveData, 3000);
};
