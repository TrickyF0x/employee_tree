#pragma once
#include <iostream>//����� �����-������
#include <queue> // ���������� ������������ ���� �������
#include <string> // ������������ ���� ��� ������ �� �������� ���� string
#include <fstream> // ���������� �����
#include <thread> //��� ������ � ��������
#include <Windows.h>

class EmployeeTree
{
public:
	struct Node // ��������� ����� ������
	{ 
		std::string key;
		std::vector<Node*> child;
	};

	void create()//�������� �������, ����� �����
	{
		bool ok = false;//��� �� ���������
		std::string confirm = "2";
		std::ifstream file("Employee.txt"); // ���� �� �������� ������
		std::queue<std::string> employeeQueue;     // ������� ������ ������� ���� string
		std::string s; // ���� ����� ������ ��������� ������
	//	std::cout << "ID main start " << std::this_thread::get_id() << std::endl;
		while (1)//������
		{
			printf("������ � ������� ��� � �������? (1 - ����� / 0 - �������) ");
			std::cin >> confirm;
			if ((confirm == "1") || (confirm == "0"))
				break;
		}
		if (confirm == "1")
			if ((file) && (file.peek() != EOF))// ��������� �� ������� �����
			{
				ok = true;//��� ������
				while (getline(file, s))// ���� �� ��������� ����� ����� ������ ��������� ������ � ���������� (s)
				{
					employeeQueue.push(s);// �������� ������� � �������
				}
				file.close(); // ��������� ���� ��� �� �� ��������� ���
			}
			else
				file_prob();//����� �� ����������
		else//������ �������� ����� �������
		{
			printf("������� ������ � ����� �������: ���, ���������, ���� ��������\n-��� ���������: ��������, �������� ��� ��������, ������� done, ��� ���������\n");
			while (s != "done")
			{
				std::getline(std::cin, s);//��������� ������
				employeeQueue.push(s);// �������� ������� � �������
			}
			ok = true;//��� ������
		}
		if (ok == true)
		{
			std::queue<std::string> tmp_q = employeeQueue;    // ������� ������ � ��������� �������

			Node *root = newNode("�������������� �������� (������ ���)");//�������������� ������, ������������ ��� �������� ������ ����

			std::thread th([&]() { print(root); });//������� ����� �� �����

			while (!tmp_q.empty())//� ������� ���������
			{
				s = tmp_q.front();//FIFO
				if (s.find("��������") != -1)
				{
					root->key = s;//������� ���������
					break;
				}
				tmp_q.pop();//������� �� �������
			}

			tmp_q = employeeQueue;//����� ��������� ��� �������
			int managers = 0; //������� ���������� 
			int workers = 0;//� ����������
			int i = 0;//���������� �� ������� ���������

			while (!tmp_q.empty())//���� ���������� � ������� ����������
			{
				s = tmp_q.front();//������� �� ��� ������� � �������� � ������
				if (s.find("��������") != -1)
				{
					(root->child).push_back(newNode(s));//2 ������� - ���������
					++managers;
				}
				if (s.find("��������") != -1)//����� ������� ����������
				{
					++workers;
				}
				tmp_q.pop();
			}
			if (managers == 0)//���� ���������� � ������� �� ���������
				(root->child).push_back(newNode("�������������� ��������� (������� ���)"));
			if (workers == 0)//���� ������� � ������� �� ���������
				(root->child[0]->child).push_back(newNode("�������������� ��������� (������� ���)"));
			else
			{
				if (workers <= managers)// ���������� �� ������ ���������
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
					if (s.find("��������") != -1)
					{
						(root->child[j]->child).push_back(newNode(s));//3 ������� - �������
						++k;
					}
					if (k >= i)
					{
						++j;
						k = 0;
					}
					if (j == managers - 1)
						if (workers % managers != 0)//�������� - ���������� ���. ������ �� 1
							++i;
					tmp_q.pop();
				}
			}
			th.join();//���������� ��������� ������ ������ ������
	//	std::cout << "ID main end " << std::this_thread::get_id() << std::endl;
		}
	}

private:
	Node *newNode(std::string key) // ������� ��� �������� ������
	{
		Node *temp = new Node;
		temp->key = key;
		return temp;
	}

	void file_prob()//�������� � ������
	{
		std::string confirm = "0";
		std::string s;
		//	std::cout << "ID prob start " << std::this_thread::get_id() << std::endl;
		while (1)//������ �� ����������� �����
		{
			printf("���� �� ���������� ��� ���� >> Employee.txt\n�������? (1 - �� / 0 - ���) ");
			std::cin >> confirm;
			if ((confirm == "1") || (confirm == "0"))
				break;
		}
		if (confirm == "1")
		{
			std::ofstream file("Employee.txt");//������� ���� � ��������� ��� �������
			printf("������� ������ � ����� �������: ���, ���������, ���� ��������\n-��� ���������: ��������, �������� ��� ��������, ������� done, ��� ���������\n");

			while (1)
			{
				std::getline(std::cin, s);
				if (s == "done")
					break;
				file << s << std::endl;
			}
			file.close(); // ��������� ����
			printf("���� ������ >> Employee.txt\n");
			create();//������������ � �������� �������
		}
		else
			create();
	}

	void print(Node *root)//������ ������
	{
		std::string on_screen = "2", to_file = "2";//��� ������ ������������
		std::ofstream tree_f("Tree.txt");//���� � ������� ����� ����������
	//	std::cout << "ID print start " << std::this_thread::get_id() << std::endl;
		while (1)//������
		{
			printf("������� ������ �� �����? (1 - �� / 0 - ���) ");
			std::cin >> on_screen;
			printf("���������� ������ � ����? (1 - �� / 0 - ���) ");
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
			tree_f << " " << root->key << std::endl;//������� � ����
			for (int i = 0; i < root->child.size(); i++)
			{
				tree_f << "	" << root->child[i]->key << std::endl;;
				for (int j = 0; j < root->child[i]->child.size(); j++)			// ������� ������� �� �������
					tree_f << "		" << root->child[i]->child[j]->key << std::endl;;
			}
			std::cout << "������ �������� � ���� >> Tree.txt" << std::endl;
		}
		if ((on_screen == "0") && (to_file == "0"))//���� ������� �������� ������������ �� �������
			printf("������ ����������\n");
		//	std::cout << "ID print end " << std::this_thread::get_id() << std::endl;
	}

};