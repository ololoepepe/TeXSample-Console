<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="ru_RU">
<context>
    <name>BSettingsNode</name>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="93"/>
        <source>Logging mode. Possible values:
0 or less - don&apos;t log
1 - log to console only
2 - log to file only
3 and more - log to console and file
The default is 2</source>
        <translation>Режим ведения логов. Возможные значения:
0 или меньше - не вести логов
1 - вести лог только в консоли
2 - вести лог только в файле
3 - вести лог и в консоли, и в файле
Значение по умолчанию - 2</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="100"/>
        <source>Logging the &quot;keep alive&quot; operations. Possible values:
0 or less - don&apos;t log
1 - log
The default is 0</source>
        <translation>Ведение логов операции &quot;keep alive&quot; (проверка соединения). Возможные значения:
0 или меньше - не вести логов
1 - вести лог локально
Значение по умолчанию - 0</translation>
    </message>
</context>
<context>
    <name>BTerminalIOHandler</name>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="106"/>
        <source>This is TeXSample Console. Enter &quot;help --all&quot; to see full Help</source>
        <translation>Это TeXSample Console. Введите &quot;help --all&quot; чтобы увидеть полную Справку</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="111"/>
        <source>Connect to a remote server. If address is not specified, texsample-server.no-ip.org is used</source>
        <translation>Подключиться к удаленному серверу. Если адрес не указан, вместо него будет использован texsample-server.no-ip.org</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="115"/>
        <source>Disconnect from the remote server</source>
        <translation>Отключиться от удалённого сервера</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="118"/>
        <source>Show for how long the server has been running</source>
        <translation>Показать, как долго работает сервер</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="122"/>
        <source>Show connected user count or list them all</source>
        <translation>Показать количество подключенных пользователей или перечислить их всех</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="125"/>
        <source>Show information about the user. The user may be specified by id or by login. Options:
  --connected-at - time when the user connected
  --info - detailed user information
  --uptime - for how long the user has been connected</source>
        <translation>Показать информацию о пользователе. Пользователь может быть указан при помощи идентификатора или логина. Опции:
  --connected-at - момент времени, когда пользователь подключился
  --info - подробная информация о пользователе
  --uptime - как долго пользователь был подключен</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="132"/>
        <source>Disconnect the specified user. If login is specified, all connections of this user will be closed</source>
        <translation>Отключить указанного пользователя. Если указан логин, то все соединения данного пользователя будут закрыты</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="137"/>
        <source>Start the server. If address is specified, the server will only listen on that address, otherwise it will listen on available all addresses.</source>
        <translation>Запустить сервер. Если указан адрес, то сервер будет принимать соединения только по этому адресу, иначе он будет принимать соединения по всем доступным адресам.</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="142"/>
        <source>Stop the server. Users are NOT disconnected</source>
        <translation>Остановить сервер. Пользователи НЕ отключаются</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="145"/>
        <source>Set the latest version of an application along with the download URL</source>
        <translation>Задать последнюю версию приложения вместе со ссылкой для загрузки</translation>
    </message>
</context>
<context>
    <name>Global</name>
    <message>
        <location filename="../src/global.cpp" line="19"/>
        <source>Enter logging mode:</source>
        <translation>Введите режим ведения логов:</translation>
    </message>
</context>
<context>
    <name>TerminalIOHandler</name>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="177"/>
        <source>Connecting to</source>
        <translation>Подключаемся к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="165"/>
        <source>Enter login:</source>
        <translation>Введите логин:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="171"/>
        <source>Enter password:</source>
        <translation>Введите пароль:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="161"/>
        <source>Already connected to</source>
        <translation>Уже подключен к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="168"/>
        <location filename="../src/terminaliohandler.cpp" line="174"/>
        <source>Operation aborted</source>
        <translation>Операция отменена</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="182"/>
        <source>Failed to connect to</source>
        <translation>Не удалось подключиться к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="185"/>
        <source>Connected to</source>
        <translation>Подключен к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="185"/>
        <source>Authorizing...</source>
        <translation>Авторизуемся...</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="196"/>
        <source>Authorization timed out</source>
        <translation>Превышено время ожидания авторизации</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="203"/>
        <location filename="../src/terminaliohandler.cpp" line="247"/>
        <location filename="../src/terminaliohandler.cpp" line="274"/>
        <location filename="../src/terminaliohandler.cpp" line="358"/>
        <location filename="../src/terminaliohandler.cpp" line="384"/>
        <location filename="../src/terminaliohandler.cpp" line="407"/>
        <source>Operation error</source>
        <translation>Ошибка операции</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="212"/>
        <source>Authorization failed. The following error occured:</source>
        <translation>Авторизация не удалась. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="216"/>
        <source>Authorized successfully with user id</source>
        <translation>Успешно авторизован с кодом пользователя</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="254"/>
        <source>Uptime:</source>
        <translation>Аптайм:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="256"/>
        <source>Failed to show uptime. The following error occured:</source>
        <translation>Не удалось показать аптайм. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="285"/>
        <source>Connected user count:</source>
        <translation>Количество подключенных пользователей:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="292"/>
        <source>Listing connected users</source>
        <translation>Перечисляем подключенных пользователей</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="294"/>
        <source>There are no connected users</source>
        <translation>Отсутствуют подключенные пользователи</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="323"/>
        <source>Uptime of</source>
        <translation>Аптайм</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="331"/>
        <source>Connection time of</source>
        <translation>Время подключения</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="339"/>
        <source>Failed to execute request. The following error occured:</source>
        <translation>Не удалось выполнить запрос. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="364"/>
        <source>Failed to set app version. The following error occured:</source>
        <translation>Не удалось задать версию приложения. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="390"/>
        <source>Failed to start server. The following error occured:</source>
        <translation>Не удалось запустить сервер. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="413"/>
        <source>Failed to stop server. The following error occured:</source>
        <translation>Не удалось остановить сервер. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="423"/>
        <source>Unknown command. Enter &quot;help --commands&quot; to see the list of available commands</source>
        <translation>Неизвестная команда. Введите &quot;help --commands&quot; чтобы посмотреть список доступных команд</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="441"/>
        <source>days</source>
        <translation>дней</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="497"/>
        <source>Error:</source>
        <translation>Ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="492"/>
        <source>Disconnected</source>
        <translation>Отключен</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="240"/>
        <location filename="../src/terminaliohandler.cpp" line="264"/>
        <location filename="../src/terminaliohandler.cpp" line="348"/>
        <location filename="../src/terminaliohandler.cpp" line="374"/>
        <location filename="../src/terminaliohandler.cpp" line="400"/>
        <source>Not authoized</source>
        <translation>Не авторизован</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="224"/>
        <source>Not connected</source>
        <translation>Не подключен</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="231"/>
        <source>Disconnect timeout, socket closed</source>
        <translation>Превышено время ожидания отключения, сокет закрыт</translation>
    </message>
</context>
<context>
    <name>main</name>
    <message>
        <location filename="../src/main.cpp" line="42"/>
        <source>This is</source>
        <translation>Это</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="50"/>
        <source>Enter &quot;help --commands&quot; to see the list of available commands</source>
        <translation>Введите &quot;help --commands&quot; чтобы посмотреть список доступных команд</translation>
    </message>
</context>
</TS>
