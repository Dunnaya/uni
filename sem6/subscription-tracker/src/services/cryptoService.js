const crypto = require('crypto');

const ALGORITHM = 'aes-256-cbc';
// key 32 bytes from .env (CRYPTO_SECRET must be 32 symbols or more)
const KEY = Buffer.from(process.env.CRYPTO_SECRET.padEnd(32).slice(0, 32));

exports.encrypt = (text) => {
  const iv = crypto.randomBytes(16);
  const cipher = crypto.createCipheriv(ALGORITHM, KEY, iv);
  const encrypted = Buffer.concat([cipher.update(text, 'utf8'), cipher.final()]);
  // saving IV as well as encrypted text (':')
  return iv.toString('hex') + ':' + encrypted.toString('hex');
};

exports.decrypt = (encryptedText) => {
  const [ivHex, dataHex] = encryptedText.split(':');
  const iv = Buffer.from(ivHex, 'hex');
  const decipher = crypto.createDecipheriv(ALGORITHM, KEY, iv);
  const decrypted = Buffer.concat([
    decipher.update(Buffer.from(dataHex, 'hex')),
    decipher.final(),
  ]);
  return decrypted.toString('utf8');
};