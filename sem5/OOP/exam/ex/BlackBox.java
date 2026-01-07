import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

// Головний клас для запуску
public class Main {
    public static void main(String[] args) {
        System.out.println("=== ЗАВДАННЯ 1: Чорна Скринька (BlackBox) ===");

        BlackBox box = new BlackBox();

        // 1. Додаємо числа у "чорну скриньку"
        box.addNumber(100);
        box.addNumber(25);
        box.addNumber(5);
        box.addNumber(50);
        
        // Маємо набір: {100, 25, 5, 50}
        // Відсортований вигляд для розуміння: {5, 25, 50, 100}
        // Індекси:                             0,  1,  2,   3

        // 2. Перевіряємо при K=0 (початкове значення)
        // Має повернути найменше число (5)
        System.out.println("К-й (0) елемент: " + box.getKthSmallest());

        // 3. Змінюємо внутрішній лічильник K на 2
        // Має повернути 50 (бо це 3-тє за величиною число: 0->5, 1->25, 2->50)
        box.setInternalK(2);
        System.out.println("К-й (2) елемент: " + box.getKthSmallest());
        
        // 4. Тест на помилку (якщо K завелике)
        try {
            box.setInternalK(10);
            box.getKthSmallest();
        } catch (Exception e) {
            System.out.println("Очікувана помилка: " + e.getMessage());
        }
    }
}

// --- НИЖЧЕ ОПИСАНИЙ КЛАС ЧОРНОЇ СКРИНЬКИ ---

class BlackBox {
    // Зберігає множину чисел. Використовуємо List, а не Set, 
    // щоб коректно обробляти дублікати (наприклад, дві п'ятірки).
    private List<Integer> numbers;
    
    // Внутрішній лічильник K
    private int k;

    public BlackBox() {
        this.numbers = new ArrayList<>();
        this.k = 0; // За умовою на початку рівний 0
    }

    // Операція додавання числа до множини
    public void addNumber(int number) {
        numbers.add(number);
    }

    // Метод для зміни внутрішнього лічильника (необхідний для демонстрації роботи)
    public void setInternalK(int k) {
        if (k < 0) {
            System.out.println("Помилка: лічильник не може бути від'ємним");
            return;
        }
        this.k = k;
    }

    // Операція повернення K-го за мінімальністю числа
    public int getKthSmallest() {
        if (numbers.isEmpty()) {
            throw new IllegalStateException("Скринька порожня, немає чисел.");
        }
        
        // Перевіряємо, чи існує такий елемент
        if (k >= numbers.size()) {
            throw new IndexOutOfBoundsException("Лічильник K (" + k + ") виходить за межі кількості чисел (" + numbers.size() + ").");
        }

        // Створюємо копію списку та сортуємо її
        // (сортуємо копію, щоб не змінювати порядок додавання в оригіналі, якщо це важливо)
        List<Integer> sortedList = new ArrayList<>(numbers);
        Collections.sort(sortedList);

        // Повертаємо елемент під індексом K
        return sortedList.get(k);
    }
}