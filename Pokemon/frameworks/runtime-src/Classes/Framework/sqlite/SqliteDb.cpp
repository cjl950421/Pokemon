#include "SqliteDb.h"

using namespace cocos2d;

namespace framework
{
	static __Dictionary g_sqliteCache;

	SqliteDb::~SqliteDb()
	{
		if (!_db)
		{
			sqlite3_close(_db);
		}
	}

	SqliteDb *SqliteDb::openSqlite(const std::string &dbPath)
	{
		std::string filePath = FileUtils::getInstance()->fullPathForFilename(dbPath.c_str());
		SqliteDb *pSqlite = (SqliteDb*)g_sqliteCache.objectForKey(filePath);

		if (pSqlite)
		{
			return pSqlite;
		}

		pSqlite = new SqliteDb();

		if (!pSqlite->initWithFile(filePath))
		{
			CC_SAFE_RELEASE_NULL(pSqlite);
			return nullptr;
		}
		pSqlite->autorelease();

		g_sqliteCache.setObject(pSqlite, filePath);

		return pSqlite;
	}

	void SqliteDb::clearConnectionCache()
	{
		g_sqliteCache.removeAllObjects();
	}

	const char *SqliteDb::getValueTypeName(cocos2d::Ref *value)
	{
		if (dynamic_cast<__String*>(value))
		{
			return "__String";
		}
		else if (dynamic_cast<__Integer*>(value))
		{
			return "__Integer";
		}
		else if (dynamic_cast<__Float*>(value))
		{
			return "__Float";
		}
		else
		{
			return "Unknown";
		}
	}

	bool SqliteDb::createTable(const std::string &tableName, __Array *columns)
	{
		std::string sql = "create table if not exists ";
		sql.append(tableName);
		sql.append("(");

		int count = columns->count();
		for (int i = 0; i < count; ++i)
		{
			__String *columnName = (__String*)columns->getObjectAtIndex(i);
			sql.append(columnName->getCString());
			sql.append(" text");
			if (i != count - 1)
			{
				sql.append(",");
			}
		}

		sql.append(")");

		return this->executeSql(sql);
	}

	bool SqliteDb::dropTable(const std::string &tableName)
	{
		std::string sql = "drop table if exists ";
		sql.append(tableName);
		bool res = this->executeSql(sql);

		return res;
	}

	bool SqliteDb::insertTable(const std::string &tableName, cocos2d::__Dictionary *keyValueDict)
	{
		std::string sql = "insert into ";
		sql.append(tableName);
		std::string keyStr = "(";
		std::string valueStr = "(";

		auto keyArray = keyValueDict->allKeys();
		int count = keyArray->count();

		for (int i = 0; i < count; ++i)
		{
			__String *key = (__String*)keyArray->getObjectAtIndex(i);
			__String *value = (__String*)keyValueDict->objectForKey(key->getCString());
			keyStr.append(key->getCString());
			valueStr.append("'");
			valueStr.append(value->getCString());
			valueStr.append("'");
			if (i != count - 1)
			{
				keyStr.append(",");
				valueStr.append(",");
			}
		}

		keyStr.append(")");
		valueStr.append(")");
		sql.append(keyStr);
		sql.append(" values ");
		sql.append(valueStr);

		return this->executeSql(sql);
	}

	bool SqliteDb::clearTable(const std::string &tableName)
	{
		return this->deleteFromTable(tableName, nullptr);
	}

	bool SqliteDb::deleteFromTable(const std::string &tableName, cocos2d::__Dictionary *conditionDict)
	{
		std::string sql = "delete from ";
		sql.append(tableName);
		sql.append(this->convertConditionDictionary(conditionDict));

		return this->executeSql(sql);
	}

	bool SqliteDb::deleteFromTable(const std::string &tableName, const std::string &columnName, const std::string &columnValue)
	{
		auto conditionDict = __Dictionary::create();
		conditionDict->setObject(__String::create(columnValue), columnName);

		return this->deleteFromTable(tableName, conditionDict);
	}

	bool SqliteDb::updateTable(const std::string &tableName, cocos2d::__Dictionary *keyValueDict, cocos2d::__Dictionary *conditionDict)
	{
		std::string sql = "update ";
		sql.append(tableName);
		sql.append(" set ");

		__Array *keyArray = keyValueDict->allKeys();
		int count = keyArray->count();

		for (int i = 0; i < count; ++i)
		{
			__String *key = (__String*)keyArray->getObjectAtIndex(i);
			__String *value = (__String*)keyValueDict->objectForKey(key->getCString());
			sql.append(key->getCString());
			sql.append("='");
			sql.append(value->getCString());
			sql.append("'");
			if (i != count - 1)
			{
				sql.append(",");
			}
		}

		sql.append(this->convertConditionDictionary(conditionDict));

		return this->executeSql(sql);
	}

	bool SqliteDb::updateTable(const std::string &tableName, cocos2d::__Dictionary *keyValueDict, const std::string &columnName, const std::string &columnValue)
	{
		auto conditionDict = __Dictionary::create();
		conditionDict->setObject(__String::create(columnValue), columnName);

		return this->updateTable(tableName, keyValueDict, conditionDict);
	}

	int SqliteDb::getRecordCount(const std::string &tableName)
	{
		std::string sql = "select count(*) from ";
		sql.append(tableName);
		CCLOG("Sqlite: %s", sql.c_str());

		sqlite3_stmt *pStatement = nullptr;
		int count = 0;
		bool res = sqlite3_prepare(_db, sql.c_str(), -1, &pStatement, nullptr);
		if (res)
		{
			while (sqlite3_step(pStatement) == SQLITE_ROW)
			{
				count = sqlite3_column_int(pStatement, 0);
			}
			sqlite3_finalize(pStatement);
		}

		return count;
	}

	__Array *SqliteDb::selectTable(const std::string &tableName, cocos2d::__Dictionary *conditionDict)
	{
		std::string sql = this->getExecuteSql(tableName, conditionDict);

		return this->executeQuery(sql);
	}

	__Array *SqliteDb::selectTable(const std::string &tableName, const std::string &columnName, const std::string &columnValue)
	{
		std::string sql = this->getExecuteSql(tableName, columnName, columnValue);

		return this->executeQuery(sql);
	}

	__Array *SqliteDb::selectTable(const std::string &tableName, const std::string &columnName, cocos2d::__Array *columnValues)
	{
		std::string sql = this->getExecuteSql(tableName, columnName, columnValues);

		return this->executeQuery(sql);
	}

	__Array *SqliteDb::selectTable(const std::string &tableName, const std::string &sql)
	{
		return this->executeQuery(sql);
	}

	bool SqliteDb::initWithFile(const std::string &file)
	{
		int res = sqlite3_open(file.c_str(), &_db);
		if (res == SQLITE_OK)
		{
			return true;
		}
		else
		{
			CCLOG("Sqlite open db: %s failed. Error code: %d", file.c_str(), res);
			return false;
		}
	}

	std::string SqliteDb::getExecuteSql(const std::string &tableName, cocos2d::__Dictionary *conditionDict)
	{
		std::string sql = "select * from ";
		sql.append(tableName);
		sql.append(this->convertConditionDictionary(conditionDict));

		return sql;
	}

	std::string SqliteDb::getExecuteSql(const std::string &tableName, const std::string &columnName, const std::string &columnValue)
	{
		auto conditionDict = __Dictionary::create();
		conditionDict->setObject(__String::create(columnValue), columnName);

		return this->getExecuteSql(tableName, conditionDict);
	}

	std::string SqliteDb::getExecuteSql(const std::string &tableName, const std::string &columnName, cocos2d::__Array *columnValues)
	{
		std::string sql = "select * from ";
		sql.append(tableName);
		sql.append(" where ");
		sql.append(columnName);
		sql.append(" in (");

		int count = columnValues->count();
		for (int i = 0; i < count; ++i)
		{
			__String *value = (__String*)columnValues->getObjectAtIndex(i);
			sql.append("'");
			sql.append(value->getCString());
			sql.append("'");
			if (i != count - 1)
			{
				sql.append(",");
			}
		}
		sql.append(")");

		return sql;
	}

	// execute sql statement
	bool SqliteDb::executeSql(const std::string &sql)
	{
		CCLOG("Sql: %s", sql.c_str());

		sqlite3_stmt *pStatement = nullptr;
		bool res = sqlite3_prepare(_db, sql.c_str(), -1, &pStatement, nullptr) == SQLITE_OK;
		if (res)
		{
			res = sqlite3_step(pStatement) == SQLITE_DONE;
			sqlite3_finalize(pStatement);
		}

		return res;
	}

	__Array *SqliteDb::executeQuery(const std::string &sql)
	{
		CCLOG("Sqlite: %s", sql.c_str());
		auto valueArray = __Array::create();
		sqlite3_stmt *pStmt = nullptr;

		bool res = sqlite3_prepare(_db, sql.c_str(), -1, &pStmt, nullptr) == SQLITE_OK;
		if (res)
		{
			while (sqlite3_step(pStmt) == SQLITE_ROW)
			{
				auto columnDict = __Dictionary::create();
				int columnNum = sqlite3_column_count(pStmt);
				for (int i = 0; i < columnNum; ++i)
				{
					int type = sqlite3_column_type(pStmt, i);

					const char *szName = sqlite3_column_name(pStmt, i);
					Ref *pValue = nullptr;
					if (type == SQLITE_INTEGER)
					{
						pValue = __Integer::create(sqlite3_column_int(pStmt, i));
					}
					else if (type == SQLITE_FLOAT)
					{
						pValue = __Float::create(sqlite3_column_int(pStmt, i));
					}
					else if (type == SQLITE_TEXT)
					{
						pValue = __String::create("");
					}
					columnDict->setObject(pValue, szName);
				}
				valueArray->addObject(columnDict);
			}
			sqlite3_finalize(pStmt);
		}

		return valueArray;
	}

	std::string SqliteDb::convertConditionDictionary(cocos2d::__Dictionary *conditionDict)
	{
		std::string conditionStr;

		if (conditionDict && conditionDict->count() > 0)
		{
			conditionStr.append(" where ");

			__Array *columnNameArray = conditionDict->allKeys();
			int count = columnNameArray->count();

			for (int i = 0; i < count; ++i)
			{
				auto columnName = (__String*)columnNameArray->getObjectAtIndex(i);
				auto columnValue = (__String*)conditionDict->objectForKey(columnName->getCString());

				conditionStr.append(columnName->getCString());
				conditionStr.append("='");
				conditionStr.append(columnValue->getCString());
				conditionStr.append("'");

				if (i != count - 1)
				{
					conditionStr.append(" and ");
				}
			}
		}
		return conditionStr;
	}
}