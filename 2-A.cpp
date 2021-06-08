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
// Max 를 510으로 둔 이유는 밑에다가 적어놓았습니다 !
// => at 41 line

using namespace std;

clock_t start1;
clock_t end1;
double TimeCheck1;
double TimeCheck2;
double TimeCheck3;
double TimeCheck4;

// B_Plus_Tree의 노드 구조체를 선언합니다.
struct Node {
  // 구조체 선언입니다.
    float data[Max];
    // 노드에 들어갈 자료의 배열입니다.
    Node *ptr[Max+1];
    // 노드 포인터의 배열.
    bool is_leaf;
    // 리프 노드인지 확인합니다.
    int n;
    // 자료의 개수를 의미합니다.
    int BlockNum[Max+1];
}*root = NULL, *new_node = NULL, *x = NULL;


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// 왜 Max 를 510 으로 했나 ??
// 블럭 하나당 노드 한개 입니다. (4096 byte)
// 그렇다면 Node 한개당 data 배열 => Max*4 byte
// 포인터 배열 => (Max + 1) * 4 byte , bool => 1 byte , int => 4 byte
// 8*Max + 9 <= 4096 이므로 Max가 되는 최대의 값은 510이다.
// 테스트 데이터가 4만개로 주어졌고, 이것에 대해선 효율이 안좋다고 생각 될 수 있으나, 주어진 데이터가 4만개가 아니라 훨씬 더 커진다면
// 노드를 블럭 하나로 생각했을때에, 블럭 하나만 읽어온다고 했을때에 그 효율은 극대화 된다고 생각합니다 !
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


Node * init() {
    // 노드를 초기화 함수
    new_node = new Node;
    // 객체를 할당합니다.
    for (int i = 0; i < Max; i++) {
        new_node->data[i] = 0.0;
    }
    for (int i = 0; i < Max+1; i++) {
        new_node->ptr[i] = NULL;
    }
    // new_node->data = {0};
    // 노드 한개가 블럭 1개 => 4096 => 레코드 34개
    // new_node->ptr = {0};
    // M + 1개의 자식 노드.
    new_node->is_leaf = true;
    // 생성된거는 기본적으로 리프 노드.
    new_node->n = 0;
    // 자료의 개수는 0개.
    for(int i = 0; i < Max+1; i++) {
        new_node->ptr[i] = NULL;
        new_node->BlockNum[i] = 0;
        // 그 노드 각각의 자식 노드(Max+1)를 초기화.
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

    // x노드를 분할하여 np1과 np3을 할당합니다.
    float mid;
    int j;
    Node *np1, *np3, *y;

    /* 분할했을때에 np1 이 부모 노드가 되고(위에),
        왼쪽이 orign_node , 오른쪽이 np3  */

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
            //왼쪽 자식 노드는 orign_node
            np1->ptr[supernode->n + 1] = np3;
            //오른쪽 자식 노드는 orign_node
            np1->n++;
        }
        else {
            np1->data[np1->n] = mid;
            //data 는 float 34개짜리 배열
            np1->ptr[np1->n] = orign_node;
            //왼쪽 자식 노드는 orign_node
            np1->ptr[np1->n + 1] = np3;
            //오른쪽 자식 노드는 orign_node
            np1->n++;
        }

        for(j = Max / 2 + 1; j < Max; j++){
        // np3는 x의 오른쪽 부분 노드를 가져감. 18부터 34까지
            np3->data[j - (Max / 2 + 1)] = orign_node->data[j];
            //np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];
            np3->n++;

            // x는 반 쪼개서 왼쪽 부분 노드만 가져감.
            orign_node->data[j] = 0.0;
            orign_node->n--;
            // 원래 데이터(orign_node)에서 왼쪽만 사용
        }
        for(j = Max / 2 + 1; j < Max+1; j++)
            np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];

        //np3->ptr[j-(Max/2+1)] = orign_node->ptr[j];

        // cout << " @@@@@ \n" << np1->n << endl;
        // for ( int k=0; k<np1->n; k++ )
        //     cout << np1->data[k] << " ";
        // cout << endl;

        // 루트 노드로 설정해줍니다.
        root = np1;
        supernode = np1;
    }
    // orign_node 의 부모 노드를 만들어준다.
    else if(i == -1) {
        // Max의 중간값(17)을 기준으로 절반으로 쪼갠다.
        mid = orign_node->data[Max / 2];
        // mid 는 orign_node 의 레코드 개수(17)
        orign_node->data[Max / 2] = 0.0;
        // 17개 레코드 0으로 초기화
        orign_node->n--;

        np1 = init();

        // np1는 부모노드임. 리프 노드 x.
        // orign_node 는 왼쪽으로 빠짐 , 리프 노드
        np1->is_leaf = false;
        orign_node->is_leaf = true;

        for(j = Max / 2 + 1; j < Max; j++){
        // np3는 x의 오른쪽 부분 노드를 가져감. 18부터 34까지
            np3->data[j - (Max / 2 + 1)] = orign_node->data[j];
            np3->BlockNum[j - (Max / 2 + 1)] = orign_node->BlockNum[j];
            np3->ptr[j - (Max / 2 + 1)] = orign_node->ptr[j];
            np3->n++;

            // x는 반 쪼개서 왼쪽 부분 노드만 가져감.
            orign_node->data[j] = 0.0;
            orign_node->n--;
            // 원래 데이터(orign_node)에서 왼쪽만 사용
        }

        // x의 모든 자식 노드를 NULL로 만듭니다.
        for(j = 0; j < Max + 1; j++) {
            orign_node->ptr[j] = NULL;
        } // x 는 리프 노드 니깐

        np1->data[0] = mid;
        //data 는 float 34개짜리 배열
        np1->ptr[np1->n] = orign_node;
        //왼쪽 자식 노드는 orign_node
        np1->ptr[np1->n + 1] = np3;
        //오른쪽 자식 노드는 orign_node
        np1->n++;

        // 루트 노드로 설정해줍니다.
        root = np1;
        supernode = np1;
    }

    // 이미 부모 노드가 있을때.
    else    {
        y = orign_node->ptr[i];
        // y는 orign_node 의 i 번째 자식 노드
        mid = y->data[Max / 2];
        y->data[Max / 2] = 0.0;
        y->n--;
        for(j = Max / 2 + 1; j < Max; j++)  {

            // np3은 x의 오른쪽 자식 부분만 가져갑니다.
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
    // 루트 노드가 NULL이라면 (비어있다면)
        root = init();
        x = root;
        supernode = root;
    }
    else{
    // 루트 노드가 NULL이 아니라면
        /*for(int j=0; j<supernode->n; j++)
            cout << supernode->data[j] << endl;
        */
        if(x->is_leaf == true && x->n == Max) {
        // 현재 리프노드이고 크기가 꽉 찬 경우 (노드에 데이터(레코드)가 32개(최대)인 경우 )
            Childnode_split(x, -1);
            //노드 분해
            x = root;
            supernode = root;
            for(i = 0; i < (x->n); i++) {
            // 값을 이동하며 삽입될 자리를 찾습니다.
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
            // 리프노드가 아닐 때입니다.
            while(x->is_leaf == false) {
            // 리프 노드까지 이동합니다.
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
                // 리프 노드 위에서도 만약에 최대치를 넘으면 분할해줍니다.
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

//weight tree 만들기
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

//height tree 만들기
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
//search 함수 ( 시작 값, 끝나는 값 ) return 은 개수
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
                                // 10K 데이터

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
        // 둘다 되는거 체크해주기

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
                                // 20K 데이터

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
        // 둘다 되는거 체크해주기

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
                                // 30K 데이터

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
        // 둘다 되는거 체크해주기

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
                                // 40K 데이터

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
        // 둘다 되는거 체크해주기

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
