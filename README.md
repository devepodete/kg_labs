# Лабораторные по компьютерной графике
![GitHub language count](https://img.shields.io/github/languages/count/devepodete/kg_labs)
![GitHub top language](https://img.shields.io/github/languages/top/devepodete/kg_labs?color=green)
![GitHub last commit](https://img.shields.io/github/last-commit/devepodete/kg_labs?color=yellow)


## Установка
0. **ЛР2 - ЛР7 + КП** написаны на C++17. Если версия ```g++``` или другого вашего компилятора не поддерживает этот язык, то необходимо будет установить ```gcc-7``` и ```g++-7``` или более новые версии.  
1. Скачайте репозиторий при помощи ```git clone``` или любым другим удобным способом.
2. Для **ЛР 1** необходимо установить библиотеки ```matplotilib, tkinter, numpy```.
3. Для **ЛР 2, ЛР 3 и ЛР 7** - SFML. На Ubuntu это можно сделать при помощи команды ```sudo apt install libsfml-dev```.
4. Для **ЛР 4-5 и ЛР 6** потребуются ```glad, glfw, glm```. Ниже опишу способ, как я устанавливал эти библиотеки.

### Установка ```glad```
1. Узнать версию OpenGL и GLSL при помощи ```glxinfo | grep "OpenGL"```. Вывод команды будет выглядеть следующим образом:

![glxinfo](https://imgur.com/yT8UGGK.png)

Поддерживаемая версия OpenGL указана после слов "OpenGL version string" (в данном примере 4.6)
Версия GLSL указана после слов "OpenGL shading language version string" (в данном примере 4.60)

2. Перейти на сайт https://glad.dav1d.de/ и указать в разделе API версию gl.
3. Нажать на кнопку "Generate" и скачать архив.
4. Создать директорию ```/usr/include/glad``` и поместить в неё файлы ```glad.h``` и ```glad.c``` из архива.
5. Заменить файл в директории ```/usr/include/KHR``` (если такая есть, если нет - создать) файлом из папки KHR в архиве.

### Установка ```glfw```
```sudo apt install libglfw3-dev```

### Установка ```glm```
```sudo apt install libglm-dev```

5. Для **КП** потребуются ```glad, glfw, glm, ImGUI```.

Нужные файлы ImGUI уже содержатся в директории с курсовой работой в папке ```imgui```. Вы также можете скачать один из последних релизов с официального гитхаб-аккаунта разработчика.

**Внимание!** Если версия GLSL не совпадает с моей (450), то необходимо в файле ```main.cpp``` в строке ```const char *glsl_version = "#version 450"``` указать нужную версию. Список доступных версий есть в начале файла ```imgui/imgui_impl_opengl3.cpp```.

### Замечания
1. Если версия OpenGL меньше 4.5, то в **ЛР 4-5**, **ЛР 6** и **КП** в файле ```main.cpp``` будет необходимо заменить вызовы функций
```c++
glNamedBufferData(VBO1, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
glNamedBufferData(EBO1, indices.size() * sizeof(unsigned), indices.data(), GL_DYNAMIC_DRAW);
```
на
```c++
glBindBuffer(GL_ARRAY_BUFFER, VBO1);
glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_DYNAMIC_DRAW);
```

2. Если версия GLSL отличается от моей, то необходимо в каждом из файлов в директории ```shaders``` в первой строке указать вашу версию.
3. Если версия CMake ниже 3.17, то необходимо в файле ```CMakeLists.txt``` в строке ```cmake_minimum_required(VERSION 3.17)``` указать вашу версию (```cmake --version```).
4. Если при компиляции появляются ошибки, связанные с санитайзером, то попросту уберите из флагов компиляции в CMakeLists.txt строки
```cmake
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address,undefined")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
```

5. В зависимости от того, как вы собираете проект, исполняемый файл может находится как в директории с исходниками, так и в директории ```cmake-build-debug``` или подобной. Это может привести к возникновению ошибки с загрузкой шейдеров в **ЛР 4-5, 6, КП**. В ```main.cpp``` вызывается исключение с сообщением
```c++
std::cout << "Failed to open file " << filename << '\n';
std::cout << "If such an error occurs, check executable file path and\n";
std::cout << "set shaders files path relatively to it" << std::endl;
```
Если исполняемый файл лежит в одной директории с файлом ```main.cpp```, то можно в коде ```main.cpp``` прописать правильные пути к файлам шейдеров, либо же добавить в CMakeLists.txt после строки ```add_executable(${PROJECT_NAME} ${SOURCES})``` строку
```cmake
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SRC_DIR})
```

## КП
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/kp/demo.gif)

Написана на ```C++```. Для работы с графикой использовал ```OpenGL```, функции загружал при помощи ```glad```. Для работы с окнами использовал ```GLFW```, для математики - ```GLM```. Для пользовательского интерфейса использовал ```ImGui```.


## ЛР 1 :chart_with_upwards_trend:
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/lab1/gif/demonstrate.gif)

Написана на ```Python```. Для совмещения виджетов с графиком использовал ```Tkinter```. Сам график рисовал при помощи ```Matplotlib```. ```Numpy``` использовал для математики.


## ЛР 2 :volcano:
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/lab2/gif/demonstrate.gif)

Написана на ```C++```. Для работы с окнами использовал ```GLFW```. API для настройки различных буферов, очередей и работы с железом - ```Vulkan```. ```GLM``` использовал для математики.


## ЛР 2 :tv:
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/lab2v2/demo.gif)

Написана на ```C++```. Для работы с графикой использовал ```SFML```. Решил переписать с нуля, так как в предыдущей реализации была хрень.


## ЛР 3 :high_brightness:
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/lab3/demo.gif)

Написана на ```C++```. Для работы с графикой использовал ```SFML```. Свет вычисляется как комбинация ambient, diffuse, specular.


## ЛР 4,5
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/lab45/demo.gif)

Написана на ```C++```. Для работы с графикой использовал ```OpenGL```, функции загружал при помощи ```glad```. Для работы с окнами использовал ```GLFW```, для математики - ```GLM```.


## ЛР 6
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/lab6/demo.gif)

Написана на ```C++```. Для работы с графикой использовал ```OpenGL```, функции загружал при помощи ```glad```. Для работы с окнами использовал ```GLFW```, для математики - ```GLM```. Фигура растягивается, у неё и источника света динамически меняется цвет.


## ЛР 7 :chart:
![Gif Demo](https://github.com/devepodete/kg_labs/blob/main/lab7/gif/demonstrate.gif)

Написана на ```C++```. Для работы с окнами и графическими примитивами использовал ```SFML```. Также написал пару своих примитивов - Checkbox и Button.
