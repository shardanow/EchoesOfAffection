# Quick Reference: DayOfSeason Calculator ???

**Quick lookup table for setting Initial Game Time**

---

## ?? Calendar Months ? DayOfSeason Conversion

### Winter Season

| Month | Day | DayOfSeason | Example Configuration |
|-------|-----|-------------|----------------------|
| **December** | 1-30 | 1-30 | Dec 1 ? DayOfSeason = 1 |
| **January** | 1-30 | 31-60 | Jan 1 ? DayOfSeason = 31 ? |
| **February** | 1-30 | 61-90 | Feb 1 ? DayOfSeason = 61 |

### Spring Season

| Month | Day | DayOfSeason | Example Configuration |
|-------|-----|-------------|----------------------|
| **March** | 1-30 | 1-30 | Mar 1 ? DayOfSeason = 1 |
| **April** | 1-30 | 31-60 | Apr 1 ? DayOfSeason = 31 |
| **May** | 1-30 | 61-90 | May 1 ? DayOfSeason = 61 |

### Summer Season

| Month | Day | DayOfSeason | Example Configuration |
|-------|-----|-------------|----------------------|
| **June** | 1-30 | 1-30 | Jun 1 ? DayOfSeason = 1 |
| **July** | 1-30 | 31-60 | Jul 1 ? DayOfSeason = 31 |
| **August** | 1-30 | 61-90 | Aug 1 ? DayOfSeason = 61 |

### Autumn Season

| Month | Day | DayOfSeason | Example Configuration |
|-------|-----|-------------|----------------------|
| **September** | 1-30 | 1-30 | Sep 1 ? DayOfSeason = 1 |
| **October** | 1-30 | 31-60 | Oct 1 ? DayOfSeason = 31 |
| **November** | 1-30 | 61-90 | Nov 1 ? DayOfSeason = 61 |

---

## ?? Quick Examples

### January 1st (New Year)
```
Season: Winter
DayOfSeason: 31    ? 30 (December days) + 1
```

### January 15th
```
Season: Winter
DayOfSeason: 45    ? 30 + 15
```

### February 1st
```
Season: Winter
DayOfSeason: 61    ? 60 (Dec+Jan) + 1
```

### February 28th
```
Season: Winter
DayOfSeason: 88    ? 60 + 28
```

### March 1st (Spring Start)
```
Season: Spring
DayOfSeason: 1     ? First day of spring
```

### July 4th
```
Season: Summer
DayOfSeason: 34    ? 30 (June) + 4
```

### December 25th (Christmas)
```
Season: Winter
DayOfSeason: 25    ? 25th day of December
```

### December 31st (New Year's Eve)
```
Season: Winter
DayOfSeason: 30    ? Last day of December
```

---

## ?? Formula

```
For months in order:

December:  DayOfSeason = DayOfMonth
January:   DayOfSeason = 30 + DayOfMonth
February:  DayOfSeason = 60 + DayOfMonth
March:     DayOfSeason = DayOfMonth
April:     DayOfSeason = 30 + DayOfMonth
May:       DayOfSeason = 60 + DayOfMonth
June:    DayOfSeason = DayOfMonth
July:      DayOfSeason = 30 + DayOfMonth
August:    DayOfSeason = 60 + DayOfMonth
September: DayOfSeason = DayOfMonth
October:   DayOfSeason = 30 + DayOfMonth
November:  DayOfSeason = 60 + DayOfMonth
```

---

## ? One-Line Reference

| Want to start on... | Set Season to... | Set DayOfSeason to... |
|---------------------|------------------|----------------------|
| January 1 | Winter | 31 |
| February 1 | Winter | 61 |
| March 1 | Spring | 1 |
| April 1 | Spring | 31 |
| May 1 | Spring | 61 |
| June 1 | Summer | 1 |
| July 1 | Summer | 31 |
| August 1 | Summer | 61 |
| September 1 | Autumn | 1 |
| October 1 | Autumn | 31 |
| November 1 | Autumn | 61 |
| December 1 | Winter | 1 |

---

**Keep this table handy when configuring your DataAsset!** ??
