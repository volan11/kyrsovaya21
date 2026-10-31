// Вариант 21. Санузел. Разбор журнала Serial Monitor и подготовка SQL
const T = 1;                                   // период опроса, с
const RANGES = { s1: [0, 100], s2: [0, 1], s3: [0, 100] };  // допустимые диапазоны

const text = String(msg.payload).replace(/^﻿/, '');
const sql = [];

text.split(/\r?\n/).forEach(raw => {
    const line = raw.trim();
    if (!line) return;
    const parts = line.split(' ');
    const kind = parts[0];                     // DATA или EVENT
    const f = {};
    parts.slice(1).forEach(p => {
        const kv = p.split('=');
        f[kv[0]] = kv[1];
    });
    const n = parseInt(f.n);
    if (isNaN(n)) return;
    const t = n * T;

    if (kind === 'DATA') {
        ['s1', 's2', 's3'].forEach(s => {
            const v = parseFloat(f[s]);
            // 1) запись в readings (если значения нет - пропускаем)
            if (!isNaN(v)) {
                sql.push(`INSERT INTO readings(n,t_s,sensor,value) VALUES(${n},${t},'${s}',${v});`);
            }
            // 2) значение отсутствует или вне диапазона - неисправность
            if (isNaN(v) || v < RANGES[s][0] || v > RANGES[s][1]) {
                sql.push(`INSERT INTO events(n,t_s,type,message) VALUES(${n},${t},'FAULT','датчик ${s} вне диапазона');`);
            }
        });
    } else if (kind === 'EVENT') {
        sql.push(`INSERT INTO events(n,t_s,type,message) VALUES(${n},${t},'${f.type}',NULL);`);
    }
});

const head = 'BEGIN;\nDELETE FROM readings;\nDELETE FROM events;\n' +
    "DELETE FROM sqlite_sequence WHERE name IN ('readings','events');\n";
return [null, { payload: head + sql.join('\n') + '\nCOMMIT;\n' }];
