/* Copyright (c) 2012-2014 LevelDOWN contributors
 * See list at <https://github.com/rvagg/node-leveldown#contributing>
 * MIT License <https://github.com/rvagg/node-leveldown/blob/master/LICENSE.md>
 */

#ifndef LD_DATABASE_H
#define LD_DATABASE_H

#include <map>
#include <vector>
#include <node.h>

#include <leveldb/cache.h>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <nan.h>

#include "leveldb_status.h"
#include "leveldown.h"
#include "iterator.h"

namespace leveldown {

const int kOk = 0;
const int kNotFound = 1;
const int kCorruption = 2;
const int kNotSupported = 3;
const int kInvalidArgument = 4;
const int kIOError = 5;
const int kNotOpened = 6;

NAN_METHOD(LevelDOWN);

struct Reference {
  v8::Persistent<v8::Object> handle;
  leveldb::Slice slice;

  Reference(v8::Local<v8::Value> obj, leveldb::Slice slice) : slice(slice) {
    v8::Local<v8::Object> _obj = NanNew<v8::Object>();
    _obj->Set(NanNew("obj"), obj);
    NanAssignPersistent(handle, _obj);
  };
};

static inline void ClearReferences (std::vector<Reference *> *references) {
  for (std::vector<Reference *>::iterator it = references->begin()
      ; it != references->end()
      ; ) {
    DisposeStringOrBufferFromSlice((*it)->handle, (*it)->slice);
    it = references->erase(it);
  }
  delete references;
}

class Database : public node::ObjectWrap {
public:
  static void Init ();
  static v8::Handle<v8::Value> NewInstance (v8::Local<v8::String> &location);

  leveldb::Status OpenDatabase (leveldb::Options* options, std::string location);
  leveldb::Status PutToDatabase (
      leveldb::WriteOptions* options
    , leveldb::Slice key
    , leveldb::Slice value
  );
  leveldb::Status GetFromDatabase (
      leveldb::ReadOptions* options
    , leveldb::Slice key
    , std::string& value
  );
  leveldb::Status DeleteFromDatabase (
      leveldb::WriteOptions* options
    , leveldb::Slice key
  );
  leveldb::Status WriteBatchToDatabase (
      leveldb::WriteOptions* options
    , leveldb::WriteBatch* batch
  );
  uint64_t ApproximateSizeFromDatabase (const leveldb::Range* range);
  void GetPropertyFromDatabase (const leveldb::Slice& property, std::string* value);
  leveldb::Iterator* NewIterator (leveldb::ReadOptions* options);
  const leveldb::Snapshot* NewSnapshot ();
  void ReleaseSnapshot (const leveldb::Snapshot* snapshot);
  void CloseDatabase ();
  NanUtf8String* Location();
  void ReleaseIterator (uint32_t id);

  Database (NanUtf8String* location);
  ~Database ();

public:
  leveldb::DB* db;
private:
  const leveldb::FilterPolicy* filterPolicy;
  leveldb::Cache* blockCache;
  NanUtf8String* location;
  uint32_t currentIteratorId;
  void(*pendingCloseWorker);

  std::map< uint32_t, leveldown::Iterator * > iterators;

  static void WriteDoing(uv_work_t *req);
  static void WriteAfter(uv_work_t *req);

  static NAN_METHOD(New);
  static NAN_METHOD(Open);
  static NAN_METHOD(Close);
  static NAN_METHOD(Put);
  static NAN_METHOD(Delete);
  static NAN_METHOD(Get);
  static NAN_METHOD(Batch);
  static NAN_METHOD(Write);
  static NAN_METHOD(Iterator);
  static NAN_METHOD(ApproximateSize);
  static NAN_METHOD(ApproximateSizeSync);
  static NAN_METHOD(GetProperty);
  static NAN_METHOD(OpenSync);
  static NAN_METHOD(PutSync);
  static NAN_METHOD(DeleteSync);
  static NAN_METHOD(BatchSync);
  static NAN_METHOD(GetSync);
  static NAN_METHOD(IsExistsSync);
  static NAN_METHOD(MultiGetSync);
  static NAN_METHOD(CloseSync);
  static NAN_METHOD(GetBufferSync);
};

} // namespace leveldown

#endif
