-- Контрольные запросы к базе db/variant.db (вариант 21, Санузел)

-- 1. Десять последних показаний (таблица readings).
SELECT id, n, t_s, sensor, value FROM readings ORDER BY id DESC LIMIT 10;

-- 2. Минимум, среднее и максимум по каждому датчику (таблица readings).
SELECT sensor, MIN(value) AS min_v, ROUND(AVG(value), 2) AS avg_v, MAX(value) AS max_v
FROM readings GROUP BY sensor ORDER BY sensor;

-- 3. Все события по порядку времени (таблица events).
SELECT id, n, t_s, type, message FROM events ORDER BY t_s, id;

-- 4. Количество записей в каждой таблице.
SELECT 'readings' AS tbl, COUNT(*) AS cnt FROM readings
UNION ALL
SELECT 'events', COUNT(*) FROM events;
