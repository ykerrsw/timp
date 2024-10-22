#include "header.h"
#include <locale>
#include <codecvt>

using namespace std;

locale loc("ru_RU.UTF-8");
wstring_convert<codecvt_utf8<wchar_t>, wchar_t> codec;

modAlphaCipher::modAlphaCipher(const string& skey) {
    for (unsigned i = 0; i < numAlpha.size(); i++)
        alphaNum[numAlpha[i]] = i;
    key = convert(getValidKey(skey));
}

string modAlphaCipher::encrypt(const string& open_text) {
    vector<int> work = convert(getValidOpenText(open_text));
    for (unsigned i = 0; i < work.size(); i++)
        work[i] = (work[i] + key[i % key.size()]) % alphaNum.size();
    return convert(work);
}

string modAlphaCipher::decrypt(const string& cipher_text) {
    vector<int> work = convert(getValidCipherText(cipher_text));
    for (unsigned i = 0; i < work.size(); i++)
        work[i] = (work[i] + alphaNum.size() - key[i % key.size()]) % alphaNum.size();
    return convert(work);
}

inline vector<int> modAlphaCipher::convert(const string& s) {
    wstring ws = codec.from_bytes(s);
    vector<int> result;
    for (auto c : ws)
        result.push_back(alphaNum[c]);
    return result;
}

inline string modAlphaCipher::convert(const vector<int>& v) {
    wstring ws;
    for (auto i : v)
        ws.push_back(numAlpha[i]);
    return codec.to_bytes(ws);
}

inline string modAlphaCipher::getValidKey(const string& s) {
    wstring ws = codec.from_bytes(s);
    if (ws.empty())
        throw cipher_error("Пустой ключ");

    for (auto& c : ws) {
        if (c < L'А' || c > L'Я')
            throw cipher_error("Неверный ключ: " + s);
        if (c >= L'а' && c <= L'я')
            c -= 32; // Приведение к верхнему регистру
    }
    
    return codec.to_bytes(ws);
}

inline string modAlphaCipher::getValidOpenText(const string& s) {
    wstring ws = codec.from_bytes(s);
    wstring tmp;

    for (auto c : ws) {
        if ((c >= L'А' && c <= L'Я') || (c >= L'а' && c <= L'я')) {
            if (c >= L'а' && c <= L'я')
                tmp.push_back(c -= 32); // Приведение к верхнему регистру
            else
                tmp.push_back(c);
        } else if (c == L' ') {
            tmp.push_back(c); // Разрешаем пробелы
        } else {
            throw cipher_error("Недопустимый символ в тексте");
        }
    }

    if (tmp.empty())
        throw cipher_error("Надо текст!"); 
    return codec.to_bytes(tmp);
}

inline string modAlphaCipher::getValidCipherText(const string& s) {
    wstring ws = codec.from_bytes(s);
    if (ws.empty())
        throw cipher_error("Empty cipher text");

    for (auto c : ws) {
        if ((c < L'А' || c > L'Я') && c != L'Ё')
            throw cipher_error("Неверный зашифрованный текст " + s);
    }

    return codec.to_bytes(ws);
}
