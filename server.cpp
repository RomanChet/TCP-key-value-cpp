#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <bits/stdc++.h>
using namespace std::chrono;
using namespace std;

// Создание мап
map <int, string> mp;
void add(int key, string value) {
	mp[key] = value;
}
// Поиск значения ключа в мп
string show(int key) {
	if (mp.find(key) != mp.end()){
    string value;
    value = mp[key];
		// если итератор не дошел до конца, то выводим значение соответствующее ключу
    return ("Найдено: Ключ " + to_string(key)) + " Значение " + value;
}
  else
		return "Ключ не существует";
}

// распарсим с указанием разделителя
string parse(string s) {
	size_t split = s.find(',');
  string result = "0";
	int key = stoi(s.substr(0, split)); // преобр-е строки в целое число
	if (split == s.npos) {
		result = show(key);
		return result;
	}
	string value = s.substr(split + 1);
	add(key, value);
  return result;
}

int main()
{
    // Создание соккета
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Не удалось создать соккет" << endl;
        return -1;
    }

    // Привязать IP-адрес и порт к сокету
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    // Сообщаем соккету что слушать
    listen(listening, SOMAXCONN);

    // Ожидание подключения
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];      // Имя удаленного клиента
    char service[NI_MAXSERV];   // Порт, к которому подключен клиент

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " подключен к порту " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " подключен к порту " << ntohs(client.sin_port) << endl;
    }

    // Завершение прослушивания соккета
    close(listening);

    // создание амссива буф, размером 4096 байт
    char buf[4096];
		string added = "Добавлено"; // объявление строк и присваивание им значений
		string badKey = "Неверный ввод";
    string test;
    string message;


    // Цикл while: принять и вернуть сообщение клиенту
    while (true)
    {
      memset(buf, 0, 4096); // заполнить 4096 "буф" байт символом '0'

      // Ожидание отправки данных клиентом
      int bytesReceived = recv(clientSocket, buf, 4096, 0);
      if (bytesReceived == -1) {
          cerr << "Ошибка приема данных" << endl;
          break;
        }
      if (bytesReceived == 0) {
          cout << "Клиент отключился " << endl;
          break;
        }
      message = string(buf, 0, bytesReceived); // присваивание строке  мессадж

      try {
        test = parse(message); // распарсивание значения от клиента и возврат кода выполнения операции
      if (test == "0"){ // если успешно (==0), то
          cout << added << endl; // вывод сообщения ДОбАВлЕнО
          strcpy(buf,added.data()); // Копирует из аддед данные в буф и возвращает буф
          send(clientSocket, buf, added.size() + 1, 0); // отправляет клиенту
          // (сокет-дескриптор, в который записываются данные ; адрес и длина буфера с записываемыми данными;
          // комбинация битовых флагов, управляющих режимами записи. Если аргумент flags равен нулю,
          // то запись в сокет (и, соответственно, считывание ) происходит в порядке поступления байтов.
          // Функция возвращает число записанных в сокет байтов
        }
      else {
          cout << test << endl;
          strcpy(buf,test.data());
      // Эхо-сообщение клиенту
          send(clientSocket, buf, test.size() + 1, 0);
          }
      }
			// Если что-то пошло не так, отработает кэтч
      catch (exception & ex) {
      cerr << ex.what() << endl;
      strcpy(buf,badKey.data());
      send(clientSocket, buf, badKey.size() + 1, 0);
      }
      }
      // Закрытие соккета
      close(clientSocket);

      return 0;
      }
