# Quick Fix: Dimension Bar показывает 50% вместо 0%

## ?? Быстрое решение (1 минута)

### Проблема
ProgressBar заполнен на 50% при значении 0

### Причина
DataAssets используют диапазон **-100 до +100**, где **0 = середина = 50%**

### Решение

1. Откройте:
   - `Content/RelationshipSystem/DataAssets/DA_Dimension_Trust`
- `Content/RelationshipSystem/DataAssets/DA_Dimension_Friendship`

2. Измените:
   ```
   Min Value: 0 (было: -100)
   Max Value: 100
   Default Value: 0
   ```

3. Сохраните и запустите игру

### Результат
? **0** ? **0%** (пустой bar)
? **50** ? **50%** (половина)
? **100** ? **100%** (полный bar)

---

?? **Подробности:** см. `DIMENSION_BAR_50_PERCENT_ISSUE.md`
