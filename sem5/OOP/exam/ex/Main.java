// Головний клас для запуску (має бути public і називатися так само, як файл)
public class Main {
    public static void main(String[] args) {
        System.out.println("=== ЗАВДАННЯ 2: Поліморфізм та Успадкування ===");

        // 1. Створюємо об'єкт (Настільна лампа)
        // Використовуємо посилання типу абстрактного класу (демонстрація поліморфізму)
        Lamp myLamp = new DeskLamp(60, "Чорна");

        // 2. Перевіряємо початковий стан
        System.out.println("Лампа увімкнена? -> " + myLamp.isOn());

        // 3. Вмикаємо (спрацює переписаний метод із DeskLamp)
        myLamp.turnOn();
        
        System.out.println("Лампа увімкнена? -> " + myLamp.isOn());

        // 4. Вимикаємо
        myLamp.turnOff();
    }
}

// --- НИЖЧЕ ОПИСАНА СТРУКТУРА КЛАСІВ ---

// 1. Інтерфейс "Джерело Світла"
interface LightSource {
    void turnOn();
    void turnOff();
    boolean isOn();
}

// 2. Абстрактний клас "Лампа"
abstract class Lamp implements LightSource {
    protected boolean isOn; // Стан (увімкнено/вимкнено)
    protected int power;    // Потужність

    public Lamp(int power) {
        this.power = power;
        this.isOn = false; // За замовчуванням вимкнена
    }

    @Override
    public void turnOn() {
        isOn = true;
        System.out.println("[Базовий клас Лампа]: Подано струм (" + power + " Вт).");
    }

    @Override
    public void turnOff() {
        isOn = false;
        System.out.println("[Базовий клас Лампа]: Струм вимкнено.");
    }

    @Override
    public boolean isOn() {
        return isOn;
    }
}

// 3. Клас "Настільна Лампа" (успадковує Лампу)
class DeskLamp extends Lamp {
    private String color;

    public DeskLamp(int power, String color) {
        super(power); // Виклик конструктора батька
        this.color = color;
    }

    // Перевизначення методу (Поліморфізм)
    // Ми розширюємо логіку: спочатку вмикається як звичайна лампа, потім додаємо ефект настільної
    @Override
    public void turnOn() {
        super.turnOn(); // Викликаємо базовий метод з класу Lamp
        System.out.println(" -> [Настільна Лампа]: " + color + " лампа яскраво освітлює зошит.");
    }
}