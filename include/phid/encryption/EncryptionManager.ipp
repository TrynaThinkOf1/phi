/* NO 'phid::' because this is included inside of a namespace guard */

#ifndef ENCRYPTIONMANAGER_IPP
#define ENCRYPTIONMANAGER_IPP

template<typename T>
void EncryptionManager::rsa_to_str(const T& key, std::string& op) const {
  CryptoPP::ByteQueue q;

  key.Save(q);

  op.resize(q.CurrentSize());
  q.Get(reinterpret_cast<byte*>(&op[0]), op.size());
}

template<typename T>
void EncryptionManager::str_to_rsa(const std::string& key, T& op) const {
  CryptoPP::ByteQueue q;

  q.Put(reinterpret_cast<const byte*>(key.data()), key.size());
  q.MessageEnd();

  op.Load(q);
}


#endif /* ENCRYPTIONMANAGER_IPP */
