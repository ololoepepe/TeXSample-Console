<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="ru_RU">
<context>
    <name>BSettingsNode</name>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="95"/>
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
        <location filename="../src/terminaliohandler.cpp" line="102"/>
        <source>Logging the &quot;keep alive&quot; operations. Possible values:
0 or less - don&apos;t log
1 - log locally
2 and more - log loaclly and remotely
The default is 0</source>
        <translation>Ведение логов операции &quot;keep alive&quot; (проверка соединения). Возможные значения:
0 или меньше - не вести логов
1 - вести лог локально
2 и более - вести лог и локально, и удалённо
Значение по умолчанию - 0</translation>
    </message>
</context>
<context>
    <name>BTerminalIOHandler</name>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="109"/>
        <source>This is TeXSample Console. Enter &quot;help --all&quot; to see full Help</source>
        <translation>Это TeXSample Console. Введите &quot;help --all&quot; чтобы увидеть полную Справку</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="114"/>
        <source>Connect to a remote server. If address is not specified, texsample-server.no-ip.org is used</source>
        <translation>Подключиться к удаленному серверу. Если адрес не указан, вместо него будет использован texsample-server.no-ip.org</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="118"/>
        <source>Disconnect from the remote server</source>
        <translation>Отключиться от удалённого сервера</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="121"/>
        <source>Show for how long the server has been running</source>
        <translation>Показать, как долго работает сервер</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="125"/>
        <source>Show connected user count or list them all</source>
        <translation>Показать количество подключенных пользователей или перечислить их всех</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="128"/>
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
        <location filename="../src/terminaliohandler.cpp" line="135"/>
        <source>Disconnect the specified user. If login is specified, all connections of this user will be closed</source>
        <translation>Отключить указанного пользователя. Если указан логин, то все соединения данного пользователя будут закрыты</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="140"/>
        <source>Start the server. If address is specified, the server will only listen on that address, otherwise it will listen on available all addresses.</source>
        <translation>Запустить сервер. Если указан адрес, то сервер будет принимать соединения только по этому адресу, иначе он будет принимать соединения по всем доступным адресам.</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="145"/>
        <source>Stop the server. Users are NOT disconnected</source>
        <translation>Остановить сервер. Пользователи НЕ отключаются</translation>
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
        <location filename="../src/terminaliohandler.cpp" line="176"/>
        <source>Connecting to</source>
        <translation>Подключаемся к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="164"/>
        <source>Enter login:</source>
        <translation>Введите логин:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="170"/>
        <source>Enter password:</source>
        <translation>Введите пароль:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="160"/>
        <source>Already connected to</source>
        <translation>Уже подключен к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="167"/>
        <location filename="../src/terminaliohandler.cpp" line="173"/>
        <source>Operation aborted</source>
        <translation>Операция отменена</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="181"/>
        <source>Failed to connect to</source>
        <translation>Не удалось подключиться к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="184"/>
        <source>Connected to</source>
        <translation>Подключен к</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="184"/>
        <source>Authorizing...</source>
        <translation>Авторизуемся...</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="195"/>
        <source>Authorization timed out</source>
        <translation>Превышено время ожидания авторизации</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="202"/>
        <location filename="../src/terminaliohandler.cpp" line="246"/>
        <location filename="../src/terminaliohandler.cpp" line="273"/>
        <location filename="../src/terminaliohandler.cpp" line="357"/>
        <location filename="../src/terminaliohandler.cpp" line="380"/>
        <source>Operation error</source>
        <translation>Ошибка операции</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="211"/>
        <source>Authorization failed. The following error occured:</source>
        <translation>Авторизация не удалась. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="215"/>
        <source>Authorized successfully with user id</source>
        <translation>Успешно авторизован с кодом пользователя</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="253"/>
        <source>Uptime:</source>
        <translation>Аптайм:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="255"/>
        <source>Failed to show uptime. The following error occured:</source>
        <translation>Не удалось показать аптайм. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="284"/>
        <source>Connected user count:</source>
        <translation>Количество подключенных пользователей:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="291"/>
        <source>Listing connected users</source>
        <translation>Перечисляем подключенных пользователей</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="293"/>
        <source>There are no connected users</source>
        <translation>Отсутствуют подключенные пользователи</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="322"/>
        <source>Uptime of</source>
        <translation>Аптайм</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="330"/>
        <source>Connection time of</source>
        <translation>Время подключения</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="338"/>
        <source>Failed to execute request. The following error occured:</source>
        <translation>Не удалось выполнить запрос. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="363"/>
        <source>Failed to start server. The following error occured:</source>
        <translation>Не удалось запустить сервер. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="386"/>
        <source>Failed to stop server. The following error occured:</source>
        <translation>Не удалось остановить сервер. Произошла следующая ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="396"/>
        <source>Unknown command. Enter &quot;help --commands&quot; to see the list of available commands</source>
        <translation>Неизвестная команда. Введите &quot;help --commands&quot; чтобы посмотреть список доступных команд</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="414"/>
        <source>days</source>
        <translation>дней</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="465"/>
        <source>Error:</source>
        <translation>Ошибка:</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="460"/>
        <source>Disconnected</source>
        <translation>Отключен</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="239"/>
        <location filename="../src/terminaliohandler.cpp" line="263"/>
        <location filename="../src/terminaliohandler.cpp" line="347"/>
        <location filename="../src/terminaliohandler.cpp" line="373"/>
        <source>Not authoized</source>
        <translation>Не авторизован</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="223"/>
        <source>Not connected</source>
        <translation>Не подключен</translation>
    </message>
    <message>
        <location filename="../src/terminaliohandler.cpp" line="230"/>
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
