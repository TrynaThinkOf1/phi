### Database Initialization Located In: phi/src/phi/Database.cpp

---

## Table: contacts
| Col | Type | Other |
| :----: | :----: | :----: |
| id | PRIMARY KEY | AUTOINCREMENT |
| name | TEXT | DEFAULT "Contact" |
| emoji | TEXT | DEFAULT "👤" |
| addr | TEXT | NOT NULL |
| shared_secret | TEXT | NOT NULL |
| rsa_key | TEXT | NOT NULL |
```sql
UNIQUE(addr)
```
 - id 0 is used to represent an unknown contact

---

## Table: messages
| Col | Type | Other |
| :----: | :----: | :----: |
| id | PRIMARY KEY | AUTOINCREMENT |
| contact_id | INTEGER | NOT NULL |
| sender | BOOLEAN | NOT NULL |
| content | TEXT | NOT NULL |
| timestamp | TEXT | NOT NULL |
| delivered | BOOLEAN | DEFAULT FALSE |
```sql
FOREIGN KEY (contact_id) REFERENCES contacts(id)
```

---

## Table: errors
| Col | Type | Other |
| :----: | :----: | :----: |
| id | PRIMARY KEY | AUTOINCREMENT |
| title | TEXT | NOT NULL |
| description | TEXT | N/A |
| timestamp | TEXT | NOT NULL |
