## Основні технології та фреймворки

### Express
JavaScript фреймворк для створення веб-застосунків та API. В проекті використовується як основний серверний фреймворк для маршрутизації і обробки HTTP-запитів.

### Node.js
Cередовище виконання JavaScript для розробки на стороні сервера. Node.js використовується для обробки запитів до сервера та взаємодії з базою даних.

### MongoDB
Документоорієнтована NoSQL база даних, яка використовується для збереження даних про продукти та користувачів.

### Mongoose
ODM (Object Data Modeling) бібліотека для MongoDB і Node.js, яка надає інтерфейс для роботи з базою даних через JavaScript обʼєкти (схеми та моделі).

### JWT (JSON Web Token)
Стандарт для створення токенів доступу, що використовується в проекті для аутентифікації і авторизації користувачів.

### bcryptjs
Бібліотека для хешування паролів, використовується для безпечного зберігання паролів користувачів.

### React
JavaScript бібліотека для створення користувацьких інтерфейсів, використовується для побудови компонентної структури застосунку.

### React Router DOM
Бібліотека для навігації в React-застосунках, надає компоненти Route, Routes, Link, Navigate і хуки useParams, useLocation, useNavigate.

### Chakra UI
UI-бібліотека для React з готовими компонентами, темами та системою стилів. Використовується для створення гарного та адаптивного інтерфейсу.

### React Icons
Бібліотека іконок для React-застосунків, використовуються набори io5, lucide, chakra та інші.

### Zustand
Бібліотека для управління станом застосунку. В проекті використовується для створення сховищ useAuthStore, useProductStore та useCartStore.

## Архітектурні компоненти

### Моделі (Models)
Структури даних, що визначають схеми документів в MongoDB:
- `Product` - модель продукту з полями name, price, image та description
- `User` - модель користувача з полями name, email, password та role

### Контролери (Controllers)
Функції, що обробляють запити та відправні відповіді:
- `product.controller.js` - управління продуктами (CRUD операції)
- `auth.controller.js` - управління користувачами (реєстрація, авторизація)

### Маршрути (Routes)
Визначають API ендпоінти та повʼязують їх із контролерами:
- `product.route.js` - маршрути для роботи з продуктами
- `user.route.js` - маршрути для авторизації та управління користувачами

### Проміжне ПЗ (Middleware)
Функції, що виконуються між запитом та відповіддю:
- `auth.middleware.js` - перевірка аутентифікації та авторизації

## Функціональні компоненти

### Система аутентифікації
Процес підтвердження особистості користувача через логін та пароль:
- `registerUser` - реєстрація нових користувачів
- `loginUser` - вхід до системи існуючих користувачів
- `getMe` - отримання інформації про поточного користувача

### Захист маршрутів
Механізми обмеження доступа до певних ендпоінтів API:
- `protect` - middleware, що перевіряє наявність та валідність JWT токена
- `authorize` - middleware для перевірки ролей користувачів

### Управління продуктами
Операції з продуктами в магазині:
- `getProducts` - отримання списку всіх продуктів
- `createProduct` - створення нового продукта
- `updateProduct` - оновлення існуючого продукта
- `deleteProduct` - видалення продукта

### Система навігації
Механізми для переміщення між сторінками застосунку:
- `Link` - компонент для навігації без перезавантаження сторінки
- `useNavigate` - хук для програмної навігації
- `useParams` - хук для доступу до параметрів URL

### Управління темами
Функціональність для перемикання між світлою і темною темами:
- `useColorMode` - хук для отримання та зміни поточної теми
- `useColorModeValue` - хук для вибору значення в залежності від теми

### Система сповіщень
Механізм для відображення інформаційних повідомлень користувачу:
- `useToast` - хук для створення сповіщень
- `toast` - функція для виведення сповіщень різних типів (success, error, warning, info)

### Модальні вікна
Компоненти для відображення додаткової інформації чи форм:
- `useDisclosure` - хук для управління станом відкриття/закриття
- `Modal` та його компоненти - система для створення діалогових вікон

### Кошик покупок
Функціональність для управління кошиком товарів:
- `addToCart` - додавання товару до кошика
- `removeFromCart` - видалення товара з кошика
- `updateQuantity` - зміна кількості товарів
- `clearCart` - очищення кошика
- `totalItems` - загальна кількість товарів
- `totalPrice` - загальна вартість товарів

## Концепції безпеки

### Хешування паролів
Процес перетворення пароля в хеш для безпечного зберігання:
- `bcrypt.hash` - функція для створення хеша пароля
- `bcrypt.compare` - функція для порівняння введеного пароля з хешом

### JWT токени
Метод аутентифікації та авторизації в форматі JSON:
- `jwt.sign` - створення підписаного токена
- `jwt.verify` - перевірка валідності токена

### Рольовий доступ (RBAC)
Система контролю доступу на основі ролей користувачів:
- Роль `user` - звичайний користувач з обмеженими правами
- Роль `admin` - адміністратор з повним доступом до керування продуктами

### Захист маршрутів
Контроль доступа на рівні користувацького інтерфейса:
- `ProtectedRoute` - компонент для обмеження доступа до певних сторінок
- `adminOnly` - параметр для обмеження доступа тільки адмінам

### Умовний рендеринг
Відображення елементів інтерфейса в залежності від стану аутентифікації:
- `isAuthenticated` - флаг для перевірки статусу аутентифікації
- `isAdmin` - флаг для перевірки наявності прав адміна

### Стилізація і користувацький інтерфейс

### Адаптивний дизайн
Техніки для створення адаптивного інтерфейса:
- `Container` - компонент для центрування і обмеження ширини контента
- `Responsive`-значення - обʼєкти для задання різних значень властивостей в залежності від розміру екрана

### Градієнти і кольори
Стилізація тексту та елементів:
- `bgGradient` - створення градієнтного фону
- `bgClip` - застосування градієнта до тексту

## Тестування

### Jest
Фреймворк для написання автоматизованих тестів JavaScript коду:
- `describe` - блок групування повʼязаних тестів
- `test`/`it` - визначення окремого теста

### SuperTest
Бібліотека для тестування HTTP-запитів:
- `request(app)` - створення тестового HTTP-запита до застосунку

### MongoDB Memory Server
Бібліотека для створення тимчасової in-memory MongoDB для тестування:
- `MongoMemoryServer` - створює тимчасову MongoDB базу даних для ізольованих тестів
