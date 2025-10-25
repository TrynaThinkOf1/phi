### Database Initialization Located In: phi/src/phid/DBManager.cpp

---

## Table: self
| Col | Type | Other |
| :----: | :----: | :----: |
| id | PRIMARY KEY | CHECK (id = 0) |
| name | TEXT | DEFAULT "Me" |
| emoji | TEXT | DEFAULT "🪞" |
| rsa_pub_key | TEXT | NOT NULL |
| rsa_priv_key | TEXT | NOT NULL |
| last_known_ip | TEXT | NOT NULL |
| hardware_profile | TEXT | DEFAULT "Unknown / Unknown" |

---

## Table: contacts
| Col | Type | Other |
| :----: | :----: | :----: |
| id | PRIMARY KEY | AUTOINCREMENT |
| name | TEXT | DEFAULT "Contact" |
| emoji | TEXT | DEFAULT "👤" |
| rsa_pub_key | TEXT | NOT NULL |
| shared_secret | TEXT | NOT NULL |
| ipv6_addr | TEXT | NOT NULL |
| hardware_profile | TEXT | NOT NULL |
```sql
UNIQUE(ipv6_addr)
```

---

## Table: messages
| Col | Type | Other |
| :----: | :----: | :----: |
| id | PRIMARY KEY | AUTOINCREMENT |
| contact_id | INTEGER | NOT NULL |
| content | TEXT | NOT NULL |
| timestamp | DATETIME | DEFAULT CURRENT_TIMESTAMP |
| is_read | BOOLEAN | DEFAULT FALSE |
| is_delivered | BOOLEAN | DEFAULT FALSE |
```sql
FOREIGN KEY(contact_id) REFERENCES contacts(id)
```

---

## Table: derrors
| Col | Type | Other |
| :----: | :----: | :----: |
| id | PRIMARY KEY | AUTOINCREMENT |
| title | TEXT | NOT NULL |
| description | TEXT | N/A |
| timestamp | DATETIME | DEFAULT CURRENT_TIMESTAMP |
| is_reported | BOOLEAN | DEFAULT FALSE |