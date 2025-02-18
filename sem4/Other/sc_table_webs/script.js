const scheduleTable = document.getElementById("schedule");
const categoriesDiv = document.getElementById("categories");

// Загружаем данные
const daysOfWeek = ["Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"];
let categories = JSON.parse(localStorage.getItem("categories")) || {
    work: { name: "💼 Работа", color: "#FFD700" },
    study: { name: "📚 Учёба", color: "#87CEFA" },
    sport: { name: "🏋️ Спорт", color: "#90EE90" },
    rest: { name: "🎮 Отдых", color: "#FFB6C1" }
};

// Загружаем расписание
function loadEvents() {
    const events = JSON.parse(localStorage.getItem("schedule")) || {};
    scheduleTable.innerHTML = "";
    
    for (let i = 0; i < 6; i++) {  // 6 строк (пример)
        const row = document.createElement("tr");

        daysOfWeek.forEach(day => {
            const cell = document.createElement("td");
            cell.dataset.day = day;
            cell.classList.add("schedule-cell");

            if (events[day] && events[day][i]) {
                cell.innerHTML = `<b>${events[day][i].time}</b><br>${events[day][i].title}`;
                cell.style.background = categories[events[day][i].category]?.color || "#ddd";
            }

            cell.onclick = () => editEvent(day, i);
            row.appendChild(cell);
        });

        scheduleTable.appendChild(row);
    }

    renderCategories();
}

// Добавляем занятие
function editEvent(day, index) {
    let time = prompt("Введите время занятия (например, 14:30):");
    if (!time) return;

    let title = prompt(`Введите занятие (${day}, ${time}):`);
    if (!title) return;

    let category;
    while (true) {
        category = prompt("Введите категорию (или создайте свою):", "study");
        if (categories[category] || Object.values(categories).some(c => c.name === category)) break;
        alert("Категория не найдена! Добавьте её вручную.");
    }

    const events = JSON.parse(localStorage.getItem("schedule")) || {};
    if (!events[day]) events[day] = {};
    events[day][index] = { time, title, category };

    localStorage.setItem("schedule", JSON.stringify(events));
    loadEvents();
}

// Добавление кастомной категории
function addCustomCategory() {
    const name = prompt("Введите название категории (например, '🚀 Проект'):");
    if (!name) return;

    const color = prompt("Введите цвет категории (например, #FF5733):");
    if (!color) return;

    const id = "custom_" + Object.keys(categories).length;
    categories[id] = { name, color };
    localStorage.setItem("categories", JSON.stringify(categories));
    loadEvents();
}

// Рисуем список категорий
function renderCategories() {
    categoriesDiv.innerHTML = "";
    Object.entries(categories).forEach(([id, category]) => {
        const span = document.createElement("span");
        span.classList.add("category-box");
        span.style.background = category.color;
        span.dataset.category = id;
        span.textContent = category.name;
        categoriesDiv.appendChild(span);
    });

    const addBtn = document.createElement("span");
    addBtn.classList.add("add-category");
    addBtn.textContent = "➕ Добавить";
    addBtn.onclick = addCustomCategory;
    categoriesDiv.appendChild(addBtn);
}

// Очистка расписания
function clearSchedule() {
    if (confirm("Вы уверены, что хотите удалить все занятия?")) {
        localStorage.removeItem("schedule");
        loadEvents();
    }
}

// Экспорт в PDF
function exportToPDF() {
    const { jsPDF } = window.jspdf;
    const doc = new jsPDF({ orientation: "landscape" });
    doc.text("Моё расписание", 10, 10);

    let y = 20;
    const events = JSON.parse(localStorage.getItem("schedule")) || {};
    daysOfWeek.forEach(day => {
        let line = `${day}: `;
        for (let i = 0; i < 6; i++) {
            if (events[day] && events[day][i]) {
                line += `[${events[day][i].time}] ${events[day][i].title} (${events[day][i].category}) `;
            }
        }
        doc.text(line, 10, y);
        y += 10;
    });

    doc.save("расписание.pdf");
}

window.onload = loadEvents;
