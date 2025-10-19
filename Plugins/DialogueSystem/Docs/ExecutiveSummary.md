# ?? Dialogue System - Executive Summary

## TL;DR

**Что это?**
Профессиональная модульная система диалогов для Unreal Engine 5, специально спроектированная для RPG и relationship simulator игр.

**Главные преимущества:**
1. ? **Полностью модульная** — 4 независимых слоя (Core ? Runtime ? UI ? AI)
2. ? **Data-Driven** — весь контент в таблицах, не в коде
3. ? **AI-Ready** — интеграция с OpenAI/Claude/Local LLMs
4. ? **Relationship System** — эмоции, доверие, романтика, память NPC
5. ? **DSL** — простой язык для условий (`affinity >= 50 && hasItem(Rose)`)
6. ? **Production-Ready** — unit tests, документация, примеры

---

## ?? Для кого эта система?

### ? Идеально подходит если:
- Вы делаете RPG или relationship simulator
- Нужны сложные разветвлённые диалоги
- Важна система отношений с NPC
- Планируется много контента (>1000 диалогов)
- Хотите AI-генерацию реплик
- Команда: программисты + писатели + дизайнеры работают параллельно

### ? Не подходит если:
- Нужны только простые линейные диалоги (overkill)
- Shooter с минимальными диалогами
- Нет ресурсов на интеграцию (хотя quick start за 5 минут)
- Нужна готовая UI (есть только базовый шаблон)

---

## ??? Архитектура в 3 предложениях

1. **Core** (чистая логика) ? парсит DataAssets, выполняет state machine
2. **Runtime** (компоненты) ? интегрируется с gameplay, управляет отношениями
3. **UI** (виджеты) ? подписывается на события и отображает

**Все слои разделены**, можно заменить UI или добавить multiplayer без изменения Core.

---

## ?? Ключевые классы

| Класс | Что делает | Пример использования |
|-------|-----------|---------------------|
| `UDialogueRunner` | State machine диалога | `Runner->StartDialogue(Asset, Player, NPC)` |
| `UDialogueContext` | Состояние мира | `Context->GetAffinity("Lianne")` |
| `UDialogueNode` | Узел графа | `Node->GetResolvedText(Context)` |
| `UDialogueComponent` | Компонент NPC | `NPC->DialogueComp->StartDialogue(Player)` |
| `URelationshipComponent` | Отношения | `Comp->ModifyAffinity(10)` |
| `UDialogueSubsystem` | Глобальный менеджер | `Subsystem->GetActiveDialogue()` |

---

## ?? Workflow в 6 шагов

```
1. Писатель создаёт диалог в Google Sheets
              ?
2. Экспорт в CSV ? Импорт в DataTable (UE Editor)
              ?
3. Создание DialogueDataAsset, валидация графа
              ?
4. Привязка к NPC через DialogueComponent
              ?
5. Игрок взаимодействует ? Runner выполняет ? UI показывает
              ?
6. Эффекты применяются к Context ? отношения меняются
```

**Итерации**: Писатель может изменить таблицу и hot-reload без перезапуска игры.

---

## ?? DSL Примеры

### Условия (Conditions)
```cpp
// Проверка отношений
affinity[Lianne] >= 50

// Проверка инвентаря
hasItem(Rose) && gold >= 100

// Комплексное условие
(romance[Lianne] >= 70 && time == Evening) || worldState(Festival.Active)

// Память
memory(FirstMeeting) && !visited(Node_Secret)
```

### Эффекты (Effects)
```cpp
// Изменить отношения
affinity[Lianne] += 10; romance[Lianne] += 5

// Дать предмет и деньги
giveItem(Sword); gold += 100

// Начать квест
startQuest(HelpLianne); setMemory(QuestStarted)

// Триггер события
triggerEvent(Cutscene_Kiss); setMood(Happy)
```

---

## ?? Relationship System

### Атрибуты
```
Affinity:  -100 (враг) ?? +100 (лучший друг)
Trust:        0 (не доверяет) ?? 100 (полное доверие)
Romance:      0 (нет чувств) ?? 100 (любовь)
Fear:         0 (не боится) ?? 100 (ужас)
Respect:      0 (презрение) ?? 100 (уважение)
Jealousy:     0 (равнодушен) ?? 100 (ревность)
```

### Настроения (Mood)
```
Neutral ? Happy ? Excited
      ?
      Sad ? Angry ? Fearful
```

### Память
```
NPC помнят:
- Посещённые узлы диалогов
- Действия игрока (подарки, оскорбления)
- Важные события (первая встреча, признание)
- Кастомные флаги (установленные эффектами)
```

---

## ?? AI Integration

### Поддерживаемые провайдеры
- **OpenAI** (GPT-4, GPT-3.5)
- **Anthropic** (Claude 3)
- **Local LLMs** (LM Studio, Ollama, Kobold)
- **Custom API** (любой OpenAI-compatible endpoint)

### Как работает
```
1. Узел типа AIGenerated достигнут
2. Система строит промпт:
   - System: "You are Lianne, a shy village girl..."
   - Context: "Time: Evening, Affinity: 75, Player gave rose"
   - Hint: "React to the gift with bashfulness"
3. Async HTTP запрос к API
4. Парсинг ответа (text + emotion)
5. Кэширование (чтобы не спамить API)
6. Fallback на статичный текст если ошибка
```

### Безопасность
- ? NSFW фильтр
- ? Rate limiting (макс 10 req/min)
- ? Timeout (3 sec)
- ? API key не в assets (Config файл)

---

## ?? Performance

### Типичные метрики
```
Operation              Time      Notes
??????????????????????????????????????????
Start dialogue         <5ms      Load + Init
Node transition        <1ms      Without AI
Condition evaluation   <0.1ms    Cached
Effect execution       <0.5ms    Basic effects
UI update              <16ms     60 FPS target
AI generation          <3s       Depends on API

Memory
??????????????????????????????????????????
1000 nodes             ~50MB
10 active dialogues    ~100MB
AI cache (1000)        ~20MB
```

### Оптимизации
- Кэширование парсинга DSL
- Object pooling для UI
- Lazy loading DataAssets
- Async AI requests

---

## ?? Best Practices Summary

### Authoring
```
? DO
- Группировать связанные эффекты
- Использовать понятные NodeId (не "Node_001")
- Проверять наличие ресурсов перед использованием
- Валидировать графы перед коммитом

? DON'T
- Циклические зависимости
- Магические числа (affinity += 37)
- Слишком длинные цепочки (>10 узлов)
- Забывать про локализацию
```

### Programming
```
? DO
- Использовать события вместо Tick
- Отделять UI от логики (MVVM)
- Писать unit tests для условий/эффектов
- Документировать публичные API

? DON'T
- Хардкодить строки
- Прямые зависимости (используйте интерфейсы)
- Блокирующие AI запросы
- Ignore валидация
```

---

## ?? Getting Started (5 минут)

```bash
# Шаг 1: Установить плагин
Копировать DialogueSystem ? YourProject/Plugins/

# Шаг 2: Включить
Edit ? Plugins ? Enable "Dialogue System" ? Restart

# Шаг 3: Создать DataAsset
Content Browser ? ПКМ ? Dialogue System ? Dialogue Data Asset

# Шаг 4: Заполнить узлы
Node Start: "Hello!"
Node End: (End type)
Edge: Start ? End

# Шаг 5: Привязать к NPC
Add Component: DialogueComponent
Set DefaultDialogue: ваш DataAsset

# Шаг 6: Test!
Play ? подойти к NPC ? нажать E
```

---

## ?? Где читать дальше?

| Если вы... | Читайте |
|-----------|---------|
| **Новичок** | [QuickStart.md](Docs/QuickStart.md) |
| **Программист** | [Architecture.md](Docs/Architecture.md) |
| **Писатель/Дизайнер** | [DSL_Reference.md](Docs/DSL_Reference.md) |
| **Интегрируете AI** | [AI_Integration.md](Docs/AI_Integration.md) |
| **Хотите всё** | [README.md](README.md) ? все ссылки |

---

## ?? Ключевые решения и почему

### 1. Почему модульная архитектура?
**Альтернатива**: монолитная система
**Выбор**: 4 модуля (Core, Runtime, UI, AI)
**Почему**: 
- Можно тестировать Core без Engine
- UI можно заменить без изменения логики
- AI опционален (не нужен в мобильной версии)
- Каждый модуль делает одну вещь (SRP)

### 2. Почему Data-Driven?
**Альтернатива**: Blueprint logic, Code-based
**Выбор**: DataAssets + DataTables
**Почему**:
- Писатели работают независимо
- Version control friendly (JSON/CSV)
- Hot reload без компиляции
- Экспорт в Excel/Sheets тривиален

### 3. Почему DSL?
**Альтернатива**: Visual scripting, Lua, Pure BP
**Выбор**: Текстовый DSL (`affinity >= 50`)
**Почему**:
- Читабельность (понятно без комментариев)
- Компактность (помещается в ячейку таблицы)
- Парсится быстро, кэшируется
- Легко расширять новыми операторами

### 4. Почему Event-Driven?
**Альтернатива**: Tick-based polling
**Выбор**: Делегаты и события
**Почему**:
- Нет лишних проверок каждый кадр
- Decoupling (UI не знает о Runner)
- Легко добавлять слушателей
- Performance (только когда нужно)

### 5. Почему Context as State?
**Альтернатива**: State в Runner, Global singletons
**Выбор**: Весь state в UDialogueContext
**Почему**:
- Stateless Runner (можно пересоздать)
- Save/Load тривиален (1 объект)
- Thread-safe для AI
- Легко тестировать (mock Context)

---

## ?? Use Cases

### Use Case 1: Dating Sim
```
- 5 героинь, каждая со своей персоной
- 50+ диалогов на героиню
- Romance, Affinity, Jealousy system
- Multiple endings (зависят от отношений)
- AI генерация для random small talk

Подходит? ? Да, идеально
```

### Use Case 2: AAA RPG
```
- 100+ NPC
- 1000+ диалогов
- Интеграция с quest system
- Voice-over для всех реплик
- Локализация на 10 языков

Подходит? ? Да, масштабируется
```

### Use Case 3: Multiplayer Co-op
```
- Диалоги должны replicateться
- Выбор одного влияет на других игроков
- Save/Load через Cloud

Подходит? ?? Частично (replication в v1.2)
```

### Use Case 4: VR Social Game
```
- Voice input от игроков
- AI генерация на лету
- Мало предопределённых диалогов

Подходит? ?? Частично (AI есть, voice input нет)
```

---

## ?? Troubleshooting

### Проблема: Диалог не запускается
```
Проверить:
1. DialogueComponent на NPC?
2. DefaultDialogue установлен?
3. CharacterId совпадает с SpeakerId в узлах?
4. Граф валиден (ValidateGraph)?
5. bCanStartDialogue = true?
```

### Проблема: UI не показывается
```
Проверить:
1. DialogueWidget добавлен на viewport?
2. BindToRunner() вызван?
3. Делегаты подписаны?
4. UI Layer не перекрыт другим виджетом?
```

### Проблема: Условия не работают
```
Проверить:
1. Синтаксис DSL корректен?
2. Context инициализирован?
3. Affinity/Items/Memory установлены?
4. Кэш не устарел (ClearCache)?
```

### Проблема: AI не генерирует
```
Проверить:
1. API key установлен в Config?
2. Интернет соединение есть?
3. Quota API не исчерпана?
4. Fallback текст присутствует?
```

---

## ? Checklist: Готов ли диалог к production?

### Content
- [ ] Все узлы заполнены (нет пустых DialogueText)
- [ ] Все Edge соединяют существующие узлы
- [ ] Start node существует
- [ ] End node достижим из Start
- [ ] Валидация графа пройдена (ValidateGraph = true)
- [ ] Локализация для всех языков

### Integration
- [ ] NPC имеет DialogueComponent
- [ ] DialogueComponent настроен (CharacterId, Dialogue)
- [ ] RelationshipComponent добавлен
- [ ] Portrait загружен
- [ ] UI виджет настроен

### Testing
- [ ] Диалог протестирован в PIE
- [ ] Все варианты выбора проверены
- [ ] Условия работают корректно
- [ ] Эффекты применяются правильно
- [ ] Save/Load работает

### Polish
- [ ] Эмоции установлены
- [ ] Voice-over записан (если есть)
- [ ] Typewriter скорость настроена
- [ ] Анимации лица/тела работают
- [ ] Звуковые эффекты добавлены

---

## ?? Что делает эту систему лучше аналогов?

### vs Dialogue Designer
```
? Более модульная (4 слоя vs монолит)
? Встроенная Relationship система
? AI интеграция out-of-the-box
? Лучше performance (event-driven)
? Нет visual editor (пока, v1.1)
```

### vs YarnSpinner
```
? Нативная интеграция с UE
? Blueprint + C++ API
? Relationship system встроен
? Data-Driven (DataAssets)
? Менее зрелый ecosystem
```

### vs Custom in-house system
```
? Production-ready (не нужно писать с нуля)
? Документирован и протестирован
? Модульный и расширяемый
? Community support (планируется)
? Regular updates
```

---

**Версия**: 1.0
**Статус**: Production Ready ??
**Время чтения**: ~10 минут
**Следующий шаг**: [QuickStart.md](Docs/QuickStart.md)
