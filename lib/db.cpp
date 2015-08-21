#include "db.hpp"
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

db::Database::Database(string fileName) : _fileName(fileName) {
	int rc = sqlite3_open(_fileName.c_str(), &_db);
	if(rc != SQLITE_OK) {
		cerr << "Database::Database: error opening db "
			<< "\"" << _fileName << "\": "
			<< "error: " << sqlite3_errmsg(_db) << endl;
		throw rc;
	}
}
db::Database::~Database() {
	if(_startTransaction) { delete _startTransaction; }
	if(_commitTransaction) { delete _commitTransaction; }
	if(_db) {
		int rc = sqlite3_close(_db);
		if(rc != SQLITE_OK) {
			cerr << "Database::~Database: error closing db "
				<< "\"" << _fileName << "\": "
				<< "error: " << sqlite3_errmsg(_db) << endl;
			throw rc;
		}
	}
}

sqlite3 *db::Database::getDB() { return _db; }
db::Transaction db::Database::transaction() {
	if(!_startTransaction) {
		_startTransaction = new Statement{*this, "BEGIN TRANSACTION"};
	}
	if(!_commitTransaction) {
		_commitTransaction = new Statement{*this, "COMMIT TRANSACTION"};
	}
	return Transaction{*_startTransaction, *_commitTransaction};
}

db::Statement::Statement(Database &db, string sql) : _db(db), _sql(sql) {
	cerr << "Statement::Statement: sql: \"" << _sql << "\"" << endl;
	const char *leftover{nullptr};
	int rc = sqlite3_prepare_v2(_db.getDB(), _sql.c_str(), _sql.size(),
			&_statement, &leftover);
	if(rc != SQLITE_OK) {
		cerr << "Statement::Statement: error preparing statement: "
			<< "\"" << _sql << "\"" << endl
			<< "    " << rc << endl;
		throw rc;
	}
	if(*leftover != '\0') {
		cerr << "Statement::Statement: leftover: " << leftover << endl;
	}
}
db::Statement::~Statement() {
	if(_statement) {
		int rc = sqlite3_finalize(_statement);
		if(rc != SQLITE_OK) {
			cerr << "Statement::~Statement: error finalizing statement: "
				<< rc << endl;
			throw rc;
		}
	}
}

void db::Statement::bind(int idx, int val) {
	sqlite3_bind_int(_statement, idx, val);
}
void db::Statement::bind(int idx, string val) {
	sqlite3_bind_text(_statement, idx, val.c_str(), val.size(), SQLITE_STATIC);
}
db::Result db::Statement::execute() { return Result(*this); }

string db::Statement::sql() const { return _sql; }
sqlite3_stmt *db::Statement::getStatement() { return _statement; }


db::Result::Result(Statement &statement) : _statement(statement) { step(); }
db::Result::~Result() { sqlite3_reset(_statement.getStatement()); }

int db::Result::status() { return _rc; }
int db::Result::step() { return _rc = sqlite3_step(_statement.getStatement()); }

int db::Result::getInteger(int idx) {
	return sqlite3_column_int(_statement.getStatement(), idx);
}
string db::Result::getString(int idx) {
	const unsigned char *result = sqlite3_column_text(_statement.getStatement(), idx);
	return string{(char *)result};
}


db::Transaction::Transaction(Statement &start, Statement &end)
		: _start(start), _end(end) {
	auto result = _start.execute();
	int rc = result.status();
	if(rc != SQLITE_DONE) { throw rc; }
}
db::Transaction::~Transaction() {
	auto result = _end.execute();
	int rc = result.status();
	if(rc != SQLITE_DONE) { throw rc; }
}

