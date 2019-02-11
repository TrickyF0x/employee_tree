#pragma once
#include <iostream>//поток ввода-вывода
#include <queue> // подключаем заголовочный файл очереди
#include <string> // заголовочный файл для работы со строками типа string
#include <fstream> // подключаем файлы
#include <thread> //для работы с потоками
#include <Windows.h>

class EmployeeTree
{
public:
	struct Node // структура узлов дерева
	{ 
		std::string key;
		std::vector<Node*> child;
	};

	void create()//основная функция, поток ввода
	{
		bool ok = false;//все ли корректно
		std::string confirm = "2";
		std::ifstream file("Employee.txt"); // файл из которого читаем
		std::queue<std::string> employeeQueue;     // создаем пустую очередь типа string
		std::string s; // сюда будем класть считанные строки
	//	std::cout << "ID main start " << std::this_thread::get_id() << std::endl;
		while (1)//диалог
		{
			printf("Работа с файлами или в консоли? (1 - файлы / 0 - консоль) ");
			std::cin >> confirm;
			if ((confirm == "1") || (confirm == "0"))
				break;
		}
		if (confirm == "1")
			if ((file) && (file.peek() != EOF))// проверяем на наличие файла
			{
				ok = true;//все хорошо
				while (getline(file, s))// пока не достигнут конец файла класть очередную строку в переменную (s)
				{
					employeeQueue.push(s);// помещаем строчки в очередь
				}
				file.close(); // закрываем файл что бы не повредить его
			}
			else
				file_prob();//файла не существует
		else//решили работать через консоль
		{
			printf("Вводите данные в любом порядке: ФИО, должность, дата рождения\n-где должность: директор, менеджер или работник, введите done, как закончите\n");
			while (s != "done")
			{
				std::getline(std::cin, s);//считываем строку
				employeeQueue.push(s);// помещаем строчки в очередь
			}
			ok = true;//все хорошо
		}
		if (ok == true)
		{
			std::queue<std::string> tmp_q = employeeQueue;    // создаем дерево и временную очередь

			Node *root = newNode("предполагаемый директор (записи нет)");//инициализируем корень, предполагаем что директор только один

			std::thread th([&]() { print(root); });//создаем поток на вывод

			while (!tmp_q.empty())//в поисках директора
			{
				s = tmp_q.front();//FIFO
				if (s.find("директор") != -1)
				{
					root->key = s;//заносим директора
					break;
				}
				tmp_q.pop();//удаляем из очереди
			}

			tmp_q = employeeQueue;//вновь заполняем доп очередь
			int managers = 0; //считаем менеджеров 
			int workers = 0;//и работников
			int i = 0;//работников на каждого менеджера

			while (!tmp_q.empty())//ищем менеджеров и считаем работников
			{
				s = tmp_q.front();//достаем из доп очереди и помещаем в дерево
				if (s.find("менеджер") != -1)
				{
					(root->child).push_back(newNode(s));//2 уровень - менеджеры
					++managers;
				}
				if (s.find("работник") != -1)//сразу считаем работников
				{
					++workers;
				}
				tmp_q.pop();
			}
			if (managers == 0)//если менеджеров в очереди не оказалось
				(root->child).push_back(newNode("предполагаемые менеджеры (записей нет)"));
			if (workers == 0)//если рабочих в очереди не оказалось
				(root->child[0]->child).push_back(newNode("предполагаемые работники (записей нет)"));
			else
			{
				if (workers <= managers)// работников на одного менеджера
					i = 1;
				else
				{
					i = workers / managers;
				}
				int j = 0, k = 0;
				tmp_q = employeeQueue;
				while ((!tmp_q.empty()) && (j < managers))
				{
					s = tmp_q.front();
					if (s.find("работник") != -1)
					{
						(root->child[j]->child).push_back(newNode(s));//3 уровень - рабочие
						++k;
					}
					if (k >= i)
					{
						++j;
						k = 0;
					}
					if (j == managers - 1)
						if (workers % managers != 0)//нечетное - последнему мен. больше на 1
							++i;
					tmp_q.pop();
				}
			}
			th.join();//дожидаемся окончания работы потока вывода
	//	std::cout << "ID main end " << std::this_thread::get_id() << std::endl;
		}
	}

private:
	Node *newNode(std::string key) // функция для создания дерева
	{
		Node *temp = new Node;
		temp->key = key;
		return temp;
	}

	void file_prob()//проблемы с файлом
	{
		std::string confirm = "0";
		std::string s;
		//	std::cout << "ID prob start " << std::this_thread::get_id() << std::endl;
		while (1)//диалог до корректного ввода
		{
			printf("Файл не существует или пуст >> Employee.txt\nСоздать? (1 - да / 0 - нет) ");
			std::cin >> confirm;
			if ((confirm == "1") || (confirm == "0"))
				break;
		}
		if (confirm == "1")
		{
			std::ofstream file("Employee.txt");//создаем файл и заполняем его данными
			printf("Вводите данные в любом порядке: ФИО, должность, дата рождения\n-где должность: директор, менеджер или работник, введите done, как закончите\n");

			while (1)
			{
				std::getline(std::cin, s);
				if (s == "done")
					break;
				file << s << std::endl;
			}
			file.close(); // закрываем файл
			printf("Файл создан >> Employee.txt\n");
			create();//возвращаемся к основной функции
		}
		else
			create();
	}

	void print(Node *root)//печать дерева
	{
		std::string on_screen = "2", to_file = "2";//для выбора пользователя
		std::ofstream tree_f("Tree.txt");//файл в который будем записывать
	//	std::cout << "ID print start " << std::this_thread::get_id() << std::endl;
		while (1)//диалог
		{
			printf("Вывести дерево на экран? (1 - да / 0 - нет) ");
			std::cin >> on_screen;
			printf("Произвести запись в файл? (1 - да / 0 - нет) ");
			std::cin >> to_file;
			if (((on_screen == "1") || (on_screen == "0")) && ((to_file == "1") || (to_file == "0")))
				break;
		}
		if (on_screen == "1")
		{
			std::cout << " " << root->key << std::endl;
			for (int i = 0; i < root->child.size(); i++)
			{
				std::cout << "	" << root->child[i]->key << std::endl;;
				for (int j = 0; j < root->child[i]->child.size(); j++)
					std::cout << "		" << root->child[i]->child[j]->key << std::endl;;
			}
		}
		if (to_file == "1")
		{
			tree_f << " " << root->key << std::endl;//выводим в файл
			for (int i = 0; i < root->child.size(); i++)
			{
				tree_f << "	" << root->child[i]->key << std::endl;;
				for (int j = 0; j < root->child[i]->child.size(); j++)			// достаем дочерей из очереди
					tree_f << "		" << root->child[i]->child[j]->key << std::endl;;
			}
			std::cout << "Данные записаны в файл >> Tree.txt" << std::endl;
		}
		if ((on_screen == "0") && (to_file == "0"))//если никаких действий пользователь не пожелал
			printf("Работа прекращена\n");
		//	std::cout << "ID print end " << std::this_thread::get_id() << std::endl;
	}

};