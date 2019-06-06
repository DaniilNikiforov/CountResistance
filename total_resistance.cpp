#include <iostream>
#include <windows.h>

using namespace std;

class ResistorTree
{
private:
    enum nodeType{POSLD,PARAL,RESISTOR};    // состояние звена дерева(элемента)
    struct Node
    {
        nodeType type;    // объект в одном из трех состояний
        float resistance;
        int number;
        struct Node* left;
        struct Node* right;
    };

    struct Node* root;    // корень дерева
    int count;    // переменная хранит количество резисторов в дереве
    int bufferN;    // вспомогательная переменная, нужна для подсчета резисторов

    void DestroyTree(struct Node* node)    // удаление постордером, рекурсивная, непосредственно для удаления дерева
    {
        if(node!=NULL)
        {
            DestroyTree(node->left);
            DestroyTree(node->right);
            delete node;
        }
    }

   void getNodeByNumber(struct Node *node,struct Node** current,int choose)    // функция поиска резистора по номеру
    {
        if(node != NULL)
        {
            if(node->type == RESISTOR)
            {
                if(bufferN == choose)
                {
                    *current = node;
                    bufferN++;    // считает порядковый номер резистора
                }
                else
                {
                    bufferN++;
                }
            }
            getNodeByNumber(node->left,current,choose);
            getNodeByNumber(node->right,current,choose);
        }
    }
    void ShowConnectionResistors(struct Node* node)    // показывает все возможные резисторы доступные для подключения
    {
        if(node != NULL)
        {
            if(node->type == RESISTOR)
            {
                cout << bufferN++ << ". R" << node->number << endl;
            }
            ShowConnectionResistors(node->left);
            ShowConnectionResistors(node->right);
        }
    }
    void PrintResistorList(struct Node* node)    // рекурсивная функция для вывода дерева
    {
        if(node!=NULL)
        {
            if(node->type == RESISTOR)
            {
                cout << bufferN++ << ". R" << node->number << "\n   Resistance: " << node->resistance << endl;
            }
            PrintResistorList(node->left);
            PrintResistorList(node->right);
        }
    }
    float getTotalResistance(struct Node* node)    // функция рекурсивно считает суммарное сопротивление
    {
        if(node!=NULL)
        {
            if(node->type == RESISTOR)
            {
                return node->resistance;
            }
            else if(node->type == PARAL)
            {
                return (getTotalResistance(node->left) * getTotalResistance(node->right))
                    / (getTotalResistance(node->left) + getTotalResistance(node->right));
            }
            else
            {
                return getTotalResistance(node->left) + getTotalResistance(node->right);
            }
        }
		return 0;
    }


    //рекурсивное получение звена подключения в котором находится нужный резистор
    //node - само дерево
    //currentCon - звено, результат
    //currentRes - резистор звено котрого мы ищем
    void getConnectionNodeByNode(struct Node *node,struct Node** currentCon,struct Node* currentRes)
    {
        if(node!=NULL)    // дно рекурсии
        {
            if(node->left!=NULL)    // если левое поддерево не пустое
            {
                if(node->left == currentRes)    // если в левом поддереве находиться наш резистор
                {
                    *currentCon = node;    // запоминаем звено
                }
            }
            if(node->right!=NULL)    // аналогично с правым
            {
                if(node->right == currentRes)
                {
                    *currentCon = node;
                }
            }

            // рекурсивный обход дерева
            getConnectionNodeByNode(node->left,currentCon,currentRes);
            getConnectionNodeByNode(node->right,currentCon,currentRes);
        }
    }

	void Inorder(struct Node* node, int level)    // функция(рекурсивная) для вывода схемы цепи
	{
	if(node != NULL)
	{
		Inorder(node->right, level+1);
		for (int i =0; i < level; i++)
		{
			cout << "		";
		}
		if(node->type == PARAL)
			cout << "Parallel connection" << endl << endl;
		else if(node->type == POSLD)
			cout << "Sequential connection" << endl << endl;
		else
			cout << 'R' << node->number << ':' << node->resistance << endl << endl ;
		Inorder(node->left, level+1);
		}
	}

public:
    ResistorTree()
    {
        root = NULL;    // указатель на корень равен нулю
        count = 0;    // в дереве ноль елементов
    }
    ~ResistorTree()
    {
        DestroyTree();
    }

    void DestroyTree()
    {
        DestroyTree(root);
        root = NULL;
    }
    void Adding(float resistance)
    {
        if(root == NULL)
        {
            root = new struct Node;
            root->left = NULL;
            root->right = NULL;
            root->type = RESISTOR;    // присваиваем тип звену дерева
            root->number = ++count;
            root->resistance = resistance;
        }
        else
        {
            cout << "\nWhich resistor do you want to connect: \n";
            bufferN = 1;
            ShowConnectionResistors(root);    // выводится список резисторов доступных для подключения
            cout << "\n > ";

            int choose;
            cin >> choose;    // вводим резистор к которому хотим подключится

            struct Node* current = NULL;
            bufferN=1;
            getNodeByNumber(root,&current,choose);    // current - указатель на резистор который мы выбрали для подключения

            if(current!=NULL)
            {
                cout << "\nChoose type of connection: \n 1.Parallel\n 2.Sequential\n > ";
                cin >> choose;

                if(choose == 1)
                {
                    current->type = PARAL;
                }
                else
                {
                    current->type = POSLD;
                }

                current->left = new struct Node;
                current->right = new struct Node;

                current->left->type = RESISTOR;
                current->right->type = RESISTOR;

                current->left->number = current->number;
                current->left->resistance = current->resistance;

                current->number = 0;    // обнуляем номер сопротивление звена, ибо он превращается в тип соединения
                current->resistance = 0;

                current->left->left = NULL;
                current->left->right = NULL;
                current->right->left = NULL;
                current->right->right = NULL;

                current->right->number = ++count;
                current->right->resistance = resistance;
            }
            else
            {
                cout << "Error! Cant find resistor with this number!\n\n";
            }
        }
    }
    void PrintResistorList()   // функция для вызова рекурсивной функции вывода дерева
    {
        cout << "\nResistor list:\n\n";

        if(root == NULL)
        {
            cout << "Tree is empty!\n\n";
        }
        else
        {
            bufferN=1;
            PrintResistorList(root);
        }
    }
    float getTotalResistance()
    {
        if(root == NULL)
        {
            return 0;
        }
        else
        {
            return getTotalResistance(root);
        }
    }
    void DeleteResistor()    // функция для удаления резистора
    {
        if(root!=NULL)    // если дерево не пустое
        {
            if(root->left == NULL && root->right == NULL)    // если есть только один резистор
            {
                delete root;    // удаляем корень
                root = NULL;
            }
            else
            {
                cout << "\nWhich resistor do you want to delete: \n";
                bufferN = 1;
                ShowConnectionResistors(root);    // выводим список резисторов
                cout << "\n > ";

                int choose;
                cin >> choose;    // выбираем

                struct Node* currentCon = NULL;
                struct Node* currentRes = NULL;
                bufferN=1;
                getNodeByNumber(root,&currentRes,choose);    // ищем этот резистор в дереве
                getConnectionNodeByNode(root,&currentCon,currentRes);    // ищем звено соединения этого резистора


                if(currentCon == NULL)    // если не нашли резистор
                {
                    cout << "Error! Cant find resistor with this number!\n\n";
                }
                else
                {
                    if(currentCon->left == currentRes)    // если нужный резистор в левой ветке найденего звена
                    {
                        delete currentCon->left;    // удаляем резистор
                        currentCon->type = currentCon->right->type;    // переносим данные из правого резистора
                        currentCon->resistance = currentCon->right->resistance;
                        currentCon->number = currentCon->right->number;
                        currentCon->left = currentCon->right->left;
                        struct Node* buffer = currentCon->right;
                        currentCon->right = currentCon->right->right;
                        delete buffer;
                    }
                    else    // если в правой
                    {
                        delete currentCon->right;
                        currentCon->type = currentCon->left->type;
                        currentCon->resistance = currentCon->left->resistance;
                        currentCon->number = currentCon->left->number;
                        currentCon->left = currentCon->left->left;
                        struct Node* buffer = currentCon->left;
                        currentCon->right = currentCon->left->right;
                        delete buffer;
                    }
                }
            }
        }
    }

	void PrintInorder()
	{
		cout << "\nScheme of tree:\n\n";
		if(root!=NULL)
		{
			Inorder(root,0);
		}
		else
		{
			cout << "Tree is empty!\n\n";
		}
	}

};

int main()
{
    ResistorTree tree;

    int menu = -1;

    while(menu!=0)
    {
        tree.PrintResistorList();    // вывод дерева
		tree.PrintInorder();
        cout << "\nTotal resistance: " << tree.getTotalResistance();    // вывод сумарного сопротивления цепи

        cout << "\n\nMENU:\n";
        cout << "1. Add new resistor\n2. Delete resistor\n0. Exit\n\n > ";
        cin >> menu;

        switch(menu)
        {
        case 1:
            float resistance;
            cout << "\n\nInput resistance > ";
            cin >> resistance;    // ввод сопротивления

            tree.Adding(resistance);    // добавление резистора
            break;
		case 2:
			tree.DeleteResistor();
			break;
		}

		cout << endl << endl << endl;
    }

    tree.DestroyTree();    // освобождение памяти (удаление дерева)

    return 0;
}
