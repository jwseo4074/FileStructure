#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <time.h>

#define Max 511
// Max �� 510���� �� ������ �ؿ��ٰ� ������ҽ��ϴ� !
// => at 41 line

using namespace std;

clock_t start1;
clock_t end1;
double TimeCheck1;
double TimeCheck2;
double TimeCheck3;
double TimeCheck4;

// B_Plus_Tree�� ��� ����ü�� �����մϴ�.
struct Node {
  // ����ü �����Դϴ�.
    float data[Max];
    // ��忡 �� �ڷ��� �迭�Դϴ�.
    Node *ptr[Max+1];
    // ��� �������� �迭.
    bool is_leaf;
    // ���� ������� Ȯ���մϴ�.
    int n;
    // �ڷ��� ������ �ǹ��մϴ�.
    int BlockNum[Max+1];
}*root = NULL, *new_node = NULL, *x = NULL;


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// �� Max �� 510 ���� �߳� ??
// �� �ϳ��� ��� �Ѱ� �Դϴ�. (4096 byte)
// �׷��ٸ� Node �Ѱ��� data �迭 => Max*4 byte
// ������ �迭 => (Max + 1) * 4 byte , bool => 1 byte , int => 4 byte
// 8*Max + 9 <= 4096 �̹Ƿ� Max�� �Ǵ� �ִ��� ���� 510�̴�.
// �׽�Ʈ �����Ͱ� 4������ �־�����, �̰Ϳ� ���ؼ� ȿ���� �����ٰ� ���� �� �� ������, �־��� �����Ͱ� 4������ �ƴ϶� �ξ� �� Ŀ���ٸ�
// ��带 �� �ϳ��� ������������, �� �ϳ��� �о�´ٰ� �������� �� ȿ���� �ش�ȭ �ȴٰ� �����մϴ� !
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


Node * init() {
    // ��带 �ʱ�ȭ �Լ�
    new_node = new Node;
    // ��ü�� �Ҵ��մϴ�.
    for (int i = 0; i < Max; i++) {
        new_node->data[i] = 0.0;
    }
    for (int i = 0; i < Max+1; i++) {
        new_node->ptr[i] = NULL;
    }
    // new_node->data = {0};
    // ��� �Ѱ��� �� 1�� => 4096 => ���ڵ� 34��
    // new_node->ptr = {0};
    // M + 1���� �ڽ� ���.
    new_node->is_leaf = true;
    // �����ȰŴ� �⺻������ ���� ���.
    new_node->n = 0;
    // �ڷ��� ������ 0��.
    for(int i = 0; i < Max+1; i++) {
        new_node->ptr[i] = NULL;
        new_node->BlockNum[i] = 0;
        // �� ��� ������ �ڽ� ���(Max+1)�� �ʱ�ȭ.
    }
    return new_node;
}

struct Qeury {
    float weight_min;
    float weight_max;
    float height_min;
    float height_max;
};

struct Qeury Qeury_struct[100];

Node *supernode = init();
Node *height_tree = init();
Node *weight_tree = init();

int size;

vector <int> result_answer;

float Childnode_split(Node *orign_node, int i) {

    // x��带 �����Ͽ� np1�� np3�� �Ҵ��մϴ�.
    float mid;
    int j;
    Node *np1, *np3, *y;

    /* ������������ np1 �� �θ� ��尡 �ǰ�(����),
        ������ orign_node , �������� np3  */

    np3 = init();
    np3->is_leaf = true;

    if (i == -2) {
        mid = orign_node->data[Max / 2];
        orign_node->data[Max / 2] = 0.0;
        orign_node->n--;

        np1 = init();
        np1->is_leaf = false;
        np1->BlockNum[0] = 0;
        np3->is_leaf = false;
        np3->BlockNum[0] = 0;
        orign_node->is_leaf = false;

        if (orign_node != supernode){
            //cout <<"@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
            for ( int k=0; k<supernode->n; k++){
                np1->data[k] = supernode->data[k];
                np1->ptr[k] = supernode->ptr[k];
                np1->n++;
            }

            np1->data[supernode->n] = mid;
            np1->ptr[supernode->n] = orign_node;
            //���� �ڽ� ���� orign_node
            np1->ptr[supernode->n + 1] = np3;
            //������ �ڽ� ���� orign_node
            np1->n++;
        }
        else {
            np1->data[np1->n] = mid;
            //data �� float 34��¥�� �迭
            np1->ptr[np1->n] = orign_node;
            //���� �ڽ� ���� orign_node
            np1->ptr[np1->n + 1] = np3;
            //������ �ڽ� ���� orign_node
            np1->n++;
        }

        for(j = Max / 2 + 1; j < Max; j++){
        // np3�� x�� ������ �κ� ��带 ������. 18���� 34����
            np3->data[j - (Max / 2 + 1)] = orign_node->data[j];
            //np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];
            np3->n++;

            // x�� �� �ɰ��� ���� �κ� ��常 ������.
            orign_node->data[j] = 0.0;
            orign_node->n--;
            // ���� ������(orign_node)���� ���ʸ� ���
        }
        for(j = Max / 2 + 1; j < Max+1; j++)
            np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];

        //np3->ptr[j-(Max/2+1)] = orign_node->ptr[j];

        // cout << " @@@@@ \n" << np1->n << endl;
        // for ( int k=0; k<np1->n; k++ )
        //     cout << np1->data[k] << " ";
        // cout << endl;

        // ��Ʈ ���� �������ݴϴ�.
        root = np1;
        supernode = np1;
    }
    // orign_node �� �θ� ��带 ������ش�.
    else if(i == -1) {
        // Max�� �߰���(17)�� �������� �������� �ɰ���.
        mid = orign_node->data[Max / 2];
        // mid �� orign_node �� ���ڵ� ����(17)
        orign_node->data[Max / 2] = 0.0;
        // 17�� ���ڵ� 0���� �ʱ�ȭ
        orign_node->n--;

        np1 = init();

        // np1�� �θ�����. ���� ��� x.
        // orign_node �� �������� ���� , ���� ���
        np1->is_leaf = false;
        orign_node->is_leaf = true;

        for(j = Max / 2 + 1; j < Max; j++){
        // np3�� x�� ������ �κ� ��带 ������. 18���� 34����
            np3->data[j - (Max / 2 + 1)] = orign_node->data[j];
            np3->BlockNum[j - (Max / 2 + 1)] = orign_node->BlockNum[j];
            np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];
            np3->n++;

            // x�� �� �ɰ��� ���� �κ� ��常 ������.
            orign_node->data[j] = 0.0;
            orign_node->n--;
            // ���� ������(orign_node)���� ���ʸ� ���
        }

        // x�� ��� �ڽ� ��带 NULL�� ����ϴ�.
        for(j = 0; j < Max + 1; j++) {
            orign_node->ptr[j] = NULL;
        } // x �� ���� ��� �ϱ�

        np1->data[0] = mid;
        //data �� float 34��¥�� �迭
        np1->ptr[np1->n] = orign_node;
        //���� �ڽ� ���� orign_node
        np1->ptr[np1->n + 1] = np3;
        //������ �ڽ� ���� orign_node
        np1->n++;

        // ��Ʈ ���� �������ݴϴ�.
        root = np1;
        supernode = np1;
    }

    // �̹� �θ� ��尡 ������.
    else    {
        y = orign_node->ptr[i];
        // y�� orign_node �� i ��° �ڽ� ���
        mid = y->data[Max / 2];
        y->data[Max / 2] = 0.0;
        y->n--;
        for(j = Max / 2 + 1; j < Max; j++)  {

            // np3�� x�� ������ �ڽ� �κи� �������ϴ�.
            np3->data[j - (Max / 2 + 1)] = y->data[j];
            np3->n++;
            y->data[j] = 0.0;
            y->n--;
        }
        for ( int k=Max-1; k>i ; k--){
            orign_node->ptr[k+1] = orign_node->ptr[k];
            orign_node->BlockNum[k+1] = orign_node->BlockNum[k];
        }
        orign_node->ptr[i] = y;
        orign_node->ptr[i + 1] = np3;
        orign_node->BlockNum[i] = y->BlockNum[i];
        orign_node->BlockNum[i + 1] = np3->BlockNum[i+1];
    }

    return mid;
}

void sort(float *swap, int n) {
    int i,j;
    float temp = 0.0;
    for(i = 0; i < n; i++) {
        for(j = i; j <= n; j++)   {
            if(swap[i] > swap[j]) {
                temp = swap[i];
                swap[i] = swap[j];
                swap[j] = temp;
            }
        }
    }
}

void display(Node* node){
   if(node!=NULL)   {
      for(int i = 0; i < node->n; i++)
         cout<<node->data[i]<<" ";
      cout<<"\n";
      if(node->is_leaf != true) {
         for(int i = 0; i < node->n+1; i++)
            display(node->ptr[i]);
      }
   }
}

void insert(float a) {
    int i;
    float temp;
    x = root;
    supernode = root;
    if(x == NULL){
    // ��Ʈ ��尡 NULL�̶�� (����ִٸ�)
        root = init();
        x = root;
        supernode = root;
    }
    else{
    // ��Ʈ ��尡 NULL�� �ƴ϶��
        /*for(int j=0; j<supernode->n; j++)
            cout << supernode->data[j] << endl;
        */
        if(x->is_leaf == true && x->n == Max) {
        // ���� ��������̰� ũ�Ⱑ �� �� ��� (��忡 ������(���ڵ�)�� 32��(�ִ�)�� ��� )
            Childnode_split(x, -1);
            //��� ����
            x = root;
            supernode = root;
            for(i = 0; i < (x->n); i++) {
            // ���� �̵��ϸ� ���Ե� �ڸ��� ã���ϴ�.
                if((a > x->data[i]) && (a < x->data[i + 1])) {
                    i++;
                    break;
                }
                else if(a < x->data[0]) {
                    break;
                }
                else {
                    continue;
                }
            }
            x = x->ptr[i];
        }
        else {
            // ������尡 �ƴ� ���Դϴ�.
            while(x->is_leaf == false) {
            // ���� ������ �̵��մϴ�.
                if (x->n == Max ){
                    //cout << " \n\n x node is full \n\n" << endl;
                    temp = Childnode_split(x, -2);
                    x->data[x->n] = temp;
                    //sort(x->data,x->n);
                    x=root;
                    supernode = root;
                }
                for(i = 0; i < (x->n); i++) {
                    if((a > x->data[i]) && (a < x->data[i + 1])) {
                        i++;
                        break;
                    }
                    else if(a < x->data[0]) {
                        break;
                    }
                    else {
                        continue;
                    }
                }
                if((x->ptr[i])->n == Max){
                // ���� ��� �������� ���࿡ �ִ�ġ�� ������ �������ݴϴ�.
                    temp = Childnode_split(x, i);
                    x->data[x->n] = temp;
                    //sort(x->data,x->n);
                    x->n++;
                    // if (supernode->n == Max){
                    //     cout << " \n\n super node is full \n\n" << endl;
                    //     temp = Childnode_split(x, -2);
                    //     x->data[x->n] = temp;
                    //     sort(x->data,x->n);
                    //     x=root;
                    //     supernode = root;
                    // }
                    //continue;
                }
                else {
                    x = x->ptr[i];
                }
            }
        }
    }
    x->data[x->n] = a;
    //sort(x->data, x->n);
    x->n++;
}

//weight tree �����
void make_weight_tree(vector <float> &v){
    //cout << " \n display weight tree \n" ;
    root = weight_tree;
    for (auto it = v.begin(); it != v.end(); it++){
       // cout << *it << endl;
      insert(*it);
  //      display(root);
    }
    //cout << " \n display weight tree \n" ;
    weight_tree = root;
    //display(weight_tree);

    //cout << " complete make weight tree " << endl;
}

//height tree �����
void make_height_tree(vector <float> &v){
    //cout << " \n display height tree \n" ;
    height_tree = init();
    root = height_tree;
    for (auto it = v.begin(); it != v.end(); it++){
    //    cout << *it << endl;
        insert(*it);
    //    display(root);
    }
    //cout << " \n display height tree \n" ;
    height_tree = root;
    //display(height_tree);
    //cout << " complete make height tree " << endl;
}

int search_answer = 0;
//search �Լ� ( ���� ��, ������ �� ) return �� ����
void weight_search( Node * selectnode, float start, float end, vector <float> &in_range_weight_vec_10K){
    float min = start;
    float max = end;
    if(selectnode!=NULL)   {
        for(int i = 0; i < selectnode->n; i++){
           // cout << "i = " << i  <<endl;
            if (min <= selectnode->data[i] && selectnode->data[i] <= max){

            //<< selectnode->ptr[i]->data[0] << endl ;

                in_range_weight_vec_10K.push_back(selectnode->data[i]);
            }
        }
        if(selectnode->is_leaf != true){
            for(int i = 0; i < selectnode->n+1; i++)
                weight_search(selectnode->ptr[i], min, max, in_range_weight_vec_10K);
        }

   }
}

void height_search( Node * selectnode, float start, float end, vector <float> &in_range_height_vec_10K){
    float min = start;
    float max = end;
    if(selectnode!=NULL)   {
        for(int i = 0; i < selectnode->n; i++)
            if (min <= selectnode->data[i] && selectnode->data[i] <= max){
                in_range_height_vec_10K.push_back(selectnode->data[i]);
            }
        if(selectnode->is_leaf != true) {
            for(int i = 0; i < selectnode->n+1; i++)
                height_search(selectnode->ptr[i], min, max,in_range_height_vec_10K);
       }
   }
}

void Is_true(vector <float> &weight_vector, int *Is_true_weight_vec_10K , vector <float> &in_range_weight_vec_10K, vector <float> &height_vector, int *Is_true_height_vec_10K, vector <float> &in_range_height_vec_10K){
    //cout << "Lets check !! ( IS_true ) " << endl;
    for (int i=0; i<size; i++){
         for( int j=0; j<in_range_weight_vec_10K.size(); j++){
            if ( weight_vector[i] == in_range_weight_vec_10K[j] ){
                //cout << "weight_vector["<<i<<"] == in_range_weight_vec_10K[" << j << "]" << endl;
                Is_true_weight_vec_10K[i] = i;
                //Is_true_weight_vec_10K.push_back(i);
                break;
            }
         }
         for( int j=0; j<in_range_height_vec_10K.size(); j++){
            if ( height_vector[i] == in_range_height_vec_10K[j] ){
                Is_true_height_vec_10K[i] = i;
                //Is_true_height_vec_10K.push_back(i);
                break;
                //cout << "height_vector["<<i<<"] == in_range_height_vec_10K[" << j << "]" << endl;
            }
         }
    }
    search_answer = 0;
    for (int i=0; i<size; i++){
        if (Is_true_weight_vec_10K[i] > 0) {
            for (int j=0 ; j<size; j++){
                if (Is_true_height_vec_10K[j] > 0) {
                    if (Is_true_weight_vec_10K[i] == Is_true_height_vec_10K[j]) {
                        search_answer++;
                        break;
                    }
                }
            }
        }
    }
    result_answer.push_back(search_answer);
    search_answer = 0;
}

int main(){

    // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        cout << "\n<< 10K Start >> \n " << endl;
                                // 10K ������

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin10K("student10K.inp");

    //int size;
    string str;
    fin10K >> size;
    float num_float;
    int num_int;
    vector <string> result;

    vector <float> weight_vec;
    vector <float> height_vec;

    while (!fin10K.eof()) {
        getline(fin10K, str);
        //cout << str << endl;
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);

            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin10K.close();

    vector <float> in_range_weight_vec;
    vector <float> in_range_height_vec;
    int Is_true_weight_vec[size];
    int Is_true_height_vec[size];

    for( int i=0 ; i<size ; i++){
        Is_true_weight_vec[i]=0;
        Is_true_height_vec[i]=0;
    }

    make_weight_tree(weight_vec);
    make_height_tree(height_vec);

    ifstream fin_qry("students.qry");
    int size_qry;
    string str_qry;
    fin_qry >> size_qry;

    vector <string> result_qry;
    int index_qry = 0;

    while (!fin_qry.eof()) {
        getline(fin_qry, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            //cout << value_qry << endl;
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_min => " << Qeury_struct[index_qry].weight_min <<endl;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_max => " << Qeury_struct[index_qry].weight_max <<endl;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_min => " << Qeury_struct[index_qry].height_min <<endl;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_max => " << Qeury_struct[index_qry].height_max <<endl;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry.close();

    start1 = clock();

    for(int i=1; i<=size_qry; i++){
        cout << "Let's go to , weight_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].weight_min " << Qeury_struct[i].weight_min << " Qeury_struct[" <<i <<"].weight_max " << Qeury_struct[i].weight_max <<endl;
        weight_search(weight_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, in_range_weight_vec);

        cout << "Let's go to , height_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].height_min " << Qeury_struct[i].height_min << " Qeury_struct[" <<i <<"].height_max " << Qeury_struct[i].height_max <<endl;
        height_search(height_tree, Qeury_struct[i].height_min, Qeury_struct[i].height_max, in_range_height_vec);


        Is_true(weight_vec,Is_true_weight_vec,in_range_weight_vec, height_vec,Is_true_height_vec,in_range_height_vec);
        // �Ѵ� �Ǵ°� üũ���ֱ�

        in_range_weight_vec.clear();
        in_range_height_vec.clear();

        for( int i=0 ; i<size ; i++){
            Is_true_weight_vec[i]=0;
            Is_true_height_vec[i]=0;
        }
    }

    end1 = clock();

    TimeCheck1 = (double)(end1 - start1) / CLOCKS_PER_SEC;

    ofstream fout_out10K("b10K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out10K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out10K.close();

    root = init();
    x = init();
    height_tree = init();
    weight_tree = init();




    // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    cout << "\n<< 20K Start >> \n " << endl;
                                // 20K ������

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin20K("student20K.inp");

    //int size;
    str = "";
    fin20K >> size;
    num_float = 0.0;
    result.clear();

    weight_vec.clear();
    height_vec.clear();

    while (!fin20K.eof()) {
        getline(fin20K, str);
        //cout << str << endl;
        string token = "";
        stringstream ss(str);
        int k = 0;
        string value = "";
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);

            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin20K.close();
    in_range_weight_vec.clear();
    in_range_height_vec.clear();
    int Is_true_weight_vec_10K[size];
    int Is_true_height_vec_10K[size];

    for( int i=0 ; i<size ; i++){
        Is_true_height_vec_10K[i]=0;
        Is_true_height_vec_10K[i]=0;
    }

    //cout << 123 <<endl;
    make_weight_tree(weight_vec);
    make_height_tree(height_vec);
    //cout << 123 <<endl;

    ifstream fin_qry_20K("students.qry");
    size_qry = 0;;
    str_qry = "";
    fin_qry_20K >> size_qry;

    result_qry.clear();
    index_qry = 0;

    while (!fin_qry_20K.eof()) {
        getline(fin_qry_20K, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            //cout << value_qry << endl;
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_min => " << Qeury_struct[index_qry].weight_min <<endl;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_max => " << Qeury_struct[index_qry].weight_max <<endl;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_min => " << Qeury_struct[index_qry].height_min <<endl;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_max => " << Qeury_struct[index_qry].height_max <<endl;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry_20K.close();

    start1 = clock();

    for(int i=1; i<=size_qry; i++){
        cout << "Let's go to , weight_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].weight_min " << Qeury_struct[i].weight_min << " Qeury_struct[" <<i <<"].weight_max " << Qeury_struct[i].weight_max <<endl;
        weight_search(weight_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, in_range_weight_vec);

        cout << "Let's go to , height_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].height_min " << Qeury_struct[i].height_min << " Qeury_struct[" <<i <<"].height_max " << Qeury_struct[i].height_max <<endl;
        height_search(height_tree, Qeury_struct[i].height_min, Qeury_struct[i].height_max, in_range_height_vec);


        Is_true(weight_vec,Is_true_weight_vec_10K,in_range_weight_vec, height_vec,Is_true_height_vec_10K,in_range_height_vec);
        // �Ѵ� �Ǵ°� üũ���ֱ�

        in_range_weight_vec.clear();
        in_range_height_vec.clear();

        for( int i=0 ; i<size ; i++){
            Is_true_weight_vec_10K[i]=0;
            Is_true_height_vec_10K[i]=0;
        }
    }

    end1 = clock();

    TimeCheck2 = (double)(end1 - start1) / CLOCKS_PER_SEC;

    ofstream fout_out20K("b20K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out20K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out20K.close();

    root = init();
    x = init();
    height_tree = init();
    weight_tree = init();


    // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        cout << "\n<< 30K Start >> \n " << endl;
                                // 30K ������

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin30K("student30K.inp");

    //int size;
    str="";
    fin30K >> size;
    num_float = 0.0;
    num_int = 0;
    result.clear();

    weight_vec.clear();
    height_vec.clear();

    while (!fin30K.eof()) {
        getline(fin30K, str);
        //cout << str << endl;
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);

            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin30K.close();

    in_range_weight_vec.clear();
    in_range_height_vec.clear();
    int Is_true_weight_vec_30K[size];
    int Is_true_height_vec_30K[size];

    for( int i=0 ; i<size ; i++){
        Is_true_weight_vec_30K[i]=0;
        Is_true_height_vec_30K[i]=0;
    }

    make_weight_tree(weight_vec);
    make_height_tree(height_vec);

    ifstream fin_qry_30K("students.qry");
    size_qry = 0;
    str_qry= 0.0 ;
    fin_qry_30K >> size_qry;

    result_qry.clear();
    index_qry = 0;

    while (!fin_qry_30K.eof()) {
        getline(fin_qry_30K, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            //cout << value_qry << endl;
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_min => " << Qeury_struct[index_qry].weight_min <<endl;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_max => " << Qeury_struct[index_qry].weight_max <<endl;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_min => " << Qeury_struct[index_qry].height_min <<endl;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_max => " << Qeury_struct[index_qry].height_max <<endl;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry_30K.close();

    start1 = clock();

    for(int i=1; i<=size_qry; i++){
        cout << "Let's go to , weight_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].weight_min " << Qeury_struct[i].weight_min << " Qeury_struct[" <<i <<"].weight_max " << Qeury_struct[i].weight_max <<endl;
        weight_search(weight_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, in_range_weight_vec);

        cout << "Let's go to , height_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].height_min " << Qeury_struct[i].height_min << " Qeury_struct[" <<i <<"].height_max " << Qeury_struct[i].height_max <<endl;
        height_search(height_tree, Qeury_struct[i].height_min, Qeury_struct[i].height_max, in_range_height_vec);


        Is_true(weight_vec,Is_true_weight_vec_30K,in_range_weight_vec, height_vec,Is_true_height_vec_30K,in_range_height_vec);
        // �Ѵ� �Ǵ°� üũ���ֱ�

        in_range_weight_vec.clear();
        in_range_height_vec.clear();

        for( int i=0 ; i<size ; i++){
            Is_true_weight_vec_30K[i]=0;
            Is_true_weight_vec_30K[i]=0;
        }
    }

    end1 = clock();

    TimeCheck3 = (double)(end1 - start1) / CLOCKS_PER_SEC;

    ofstream fout_out30K("b30K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out30K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out30K.close();

    root = init();
    x = init();
    height_tree = init();
    weight_tree = init();

    // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    cout << "\n<< 40K Start >> \n " << endl;
                                // 40K ������

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    ifstream fin40K("student40K.inp");

    //int size;
    str = "";
    fin40K >> size;
    num_float = 0.0;
    num_int = 0;
    result.clear();

    weight_vec.clear();
    height_vec.clear();

    while (!fin40K.eof()) {
        getline(fin40K, str);
        //cout << str << endl;
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')){
            result.push_back(token);
            value = *(result.end() - 1);

            if (k == 3) {
                num_float = stof(value);
                weight_vec.push_back(num_float);
            }
            if (k == 4) {
                num_float = stof(value);
                height_vec.push_back(num_float);
            }
            k++;
        }
        result.clear();
    }
    fin40K.close();

    in_range_weight_vec.clear();
    in_range_height_vec.clear();
    int Is_true_weight_vec_40K[size];
    int Is_true_height_vec_40K[size];

    for( int i=0 ; i<size ; i++){
        Is_true_weight_vec_40K[i]=0;
        Is_true_height_vec_40K[i]=0;
    }

    make_weight_tree(weight_vec);
    make_height_tree(height_vec);

    ifstream fin_qry40K("students.qry");
    size_qry = 0;
    str_qry= "";
    fin_qry40K >> size_qry;

    result_qry.clear();
    index_qry = 0;

    while (!fin_qry40K.eof()) {
        getline(fin_qry40K, str_qry);
        str_qry.replace(0,3,"");
        string token_qry;
        stringstream ss_qry(str_qry);
        int k = 0;
        string value_qry;

        while (getline(ss_qry, token_qry, ',')){
            result_qry.push_back(token_qry);
            value_qry = *(result_qry.end() - 1);
            //cout << value_qry << endl;
            if (k == 0) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_min => " << Qeury_struct[index_qry].weight_min <<endl;
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].weight_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].weight_max => " << Qeury_struct[index_qry].weight_max <<endl;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_min = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_min => " << Qeury_struct[index_qry].height_min <<endl;
            }
            if (k == 3) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].height_max = num_float;
                //cout << " Qeury_struct[" << index_qry <<" ].height_max => " << Qeury_struct[index_qry].height_max <<endl;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry40K.close();

    start1 = clock();

    for(int i=1; i<=size_qry; i++){
        cout << "Let's go to , weight_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].weight_min " << Qeury_struct[i].weight_min << " Qeury_struct[" <<i <<"].weight_max " << Qeury_struct[i].weight_max <<endl;
        weight_search(weight_tree, Qeury_struct[i].weight_min, Qeury_struct[i].weight_max, in_range_weight_vec);

        cout << "Let's go to , height_search, " << i << " of qeury" <<endl;
        //cout << " Qeury_struct[" <<i <<"].height_min " << Qeury_struct[i].height_min << " Qeury_struct[" <<i <<"].height_max " << Qeury_struct[i].height_max <<endl;
        height_search(height_tree, Qeury_struct[i].height_min, Qeury_struct[i].height_max, in_range_height_vec);


        Is_true(weight_vec,Is_true_weight_vec_40K,in_range_weight_vec, height_vec,Is_true_height_vec_40K,in_range_height_vec);
        // �Ѵ� �Ǵ°� üũ���ֱ�

        in_range_weight_vec.clear();
        in_range_height_vec.clear();

        for( int i=0 ; i<size ; i++){
            Is_true_weight_vec_40K[i]=0;
            Is_true_height_vec_40K[i]=0;
        }
    }

    end1 = clock();

    TimeCheck4 = (double)(end1 - start1) / CLOCKS_PER_SEC;

    ofstream fout_out40K("b40K.out");
    for(int i=0; i<result_answer.size(); i++) {
        fout_out40K << result_answer[i] << "\n";
    }
    result_answer.clear();
    fout_out40K.close();

    cout << "TimeCheck1 = " << TimeCheck1 << endl;
    cout << "TimeCheck2 = " << TimeCheck2 << endl;
    cout << "TimeCheck3 = " << TimeCheck3 << endl;
    cout << "TimeCheck4 = " << TimeCheck4 << endl;

    return 0;
}
