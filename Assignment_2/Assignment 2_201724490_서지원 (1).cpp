#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#define Max 510
// Max 를 510으로 둔 이유는 밑에다가 적어놓았습니다 !
// => at 41 line

using namespace std;

struct Student{
    char name[80];
    char ID[10];
    float GPA;
    float weight;
    float height;
    int age;
    char IBG[14] = {0,};

    // 구조체 하나당 할당된 메모리 -> 106    , 14의 IBG
    // 블럭 한개가 4096 이므로 컴팩트하게 34개의 구조체가 한 블럭에 들어감 !!
};

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
        // 그 노드 각각의 자식 노드(Max+1)를 초기화.
    }
    return new_node;
}

struct Qeury {
    float min;
    float max;
    string iswhat;
};

struct Qeury Qeury_struct[100];

Node *supernode = init();
Node *GPA_tree = init();
Node *height_tree = init();
Node *weight_tree = init();

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
        np3->is_leaf = false;
        orign_node->is_leaf = false;

        if (orign_node != supernode){

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
        }
        orign_node->ptr[i] = y;
        orign_node->ptr[i + 1] = np3;
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
                    sort(x->data,x->n);
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
                    sort(x->data,x->n);
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
    sort(x->data, x->n);
    x->n++;
}



void make_GPA_tree(vector <float> &v){
    //cout << " \n display GPA tree \n" ;
    root = GPA_tree;
    for (auto it = v.begin(); it != v.end(); it++){
        //cout << *it << endl;
        insert(*it);
        //display(root);
    }
    //cout << " \n display GPA tree \n" ;
    GPA_tree = root;
    //display(GPA_tree);
}

//weight tree 만들기
void make_weight_tree(vector <float> &v){
    //cout << " \n display weight tree \n" ;
    root = weight_tree;
    for (auto it = v.begin(); it != v.end(); it++){
    //    cout << *it << endl;
        insert(*it);
     //   display(root);
    }
    //cout << " \n display weight tree \n" ;
    weight_tree = root;
    //display(weight_tree);
}

//height tree 만들기
void make_height_tree(vector <float> &v){
    //cout << " \n display height tree \n" ;
    root = height_tree;
    for (auto it = v.begin(); it != v.end(); it++){
    //    cout << *it << endl;
        insert(*it);
    //    display(root);
    }
    //cout << " \n display height tree \n" ;
    height_tree = root;
    //display(height_tree);
}

int search_answer = 0;
//search 함수 ( 시작 값, 끝나는 값 ) return 은 개수
void search( Node * selectnode, float start, float end){
    float min = start;
    float max = end;
    if(selectnode!=NULL)   {
        for(int i = 0; i < selectnode->n; i++)
            if (min <= selectnode->data[i] && selectnode->data[i] <= max){
                search_answer++;
                //cout << search_answer << endl;
            }

        if(selectnode->is_leaf != true) {
            for(int i = 0; i < selectnode->n+1; i++)
                search(selectnode->ptr[i], min, max);
      }
   }
}

struct Student Student_struct[1500][34];
int row_index = 0;
int col_index = 0;

int main(){
    ifstream fin("students1.inp");
    ofstream fout("students1.bin", ios::out | ios::binary);

    int size;
    string str;
    fin >> size;
    float num_float;
    int num_int;
    vector <string> result;
    row_index = 0;
    col_index = 0;
    int ak=0;
    while (!fin.eof()) {
        getline(fin, str);     // 구조체 하나 읽어오기.
        //cout << str << endl;
        string token;
        stringstream ss(str);
        int k = 0;
        string value;

        cout << ak++ << endl;

        while (getline(ss, token, ',')){     // 구조체 한개 안에서 값들 6개 차례대로 읽어오기
            result.push_back(token);
            value = *(result.end() - 1);
            if (k == 0) {
                const char* c = value.c_str();
                strcpy(Student_struct[row_index][col_index].name,c);
            }
            else if (k == 1) {
                const char* c = value.c_str();
                strcpy(Student_struct[row_index][col_index].ID, c);
            }
            else if (k == 2) {
                num_float = stof(value);
                Student_struct[row_index][col_index].GPA = num_float;
            }
            else if (k == 3) {
                num_float = stof(value);
                Student_struct[row_index][col_index].weight = num_float;
            }
            else if (k == 4) {
                num_float = stof(value);
                Student_struct[row_index][col_index].height = num_float;
            }
            else if (k == 5) {
                num_int = stoi(value);
                Student_struct[row_index][col_index].age = num_int;
            }
            k++;
        }
        col_index++;        // 한칸 밑으로
        if (col_index == 34) {    // row 38개 꽉차면 한칸 밑으로
            row_index++;
            col_index = 0;
        }
        result.clear();      // ex >  구조체 하나 => [0][1]  , 꽉찼으니 벡터 초기화
    }
    // 읽고 저장 끝.
    fin.close();
    cout << 1 ;
    // 바이너리 파일로 출력 !!
    for (int i = 0; i < 1200; i++) {
        for (int j = 0; j < 38; j++) {
            if ( i == 0 && j == 0 ) continue;
            fout.write((char*)&Student_struct[i][j], sizeof(Student_struct[i][j]));
        }
    }       //바이너리로 저장
    fout.close();

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    바이너리 파일로 저장 끝마친 상태 (students.bin 에 컴팩트하게 블록단위로 binary로 바껴진 값들이 다 들어가있음)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

    vector <float> GPA_vec;
    vector <float> weight_vec;
    vector <float> height_vec;

    ifstream fin_bin("students1.bin",ios::binary);
    cout <<2;
    for (int i = 0; i < 1200; i++) {
       for (int j = 0; j < 38; j++) {
          if ( i == 0 && j == 0 ) continue;
           fin_bin.read((char*)&Student_struct[i][j], sizeof(Student_struct[i][j]));
           GPA_vec.push_back(Student_struct[i][j].GPA);
           weight_vec.push_back(Student_struct[i][j].weight);
           height_vec.push_back(Student_struct[i][j].height);
       }
    }
    sort(GPA_vec.begin(), GPA_vec.end());
    sort(weight_vec.begin() , weight_vec.end());
    sort(height_vec.begin(), height_vec.end());
    fin_bin.close();

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    각각의 벡터에다가 값들 다 넣어놓은 상태  ( GPA_vec , weight_vec , height_vec )
    이제 여기서 값들 꺼내와서 비 플러스 트리 ( insert ) 만들면 됨.
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

    make_GPA_tree(GPA_vec);
    make_weight_tree(weight_vec);
    make_height_tree(height_vec);

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    3개의 트리를 각각 만들어준 상태 ( GPA_tree , height_tree , weight_tree )
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
    cout <<3;
    ifstream fin_qry("students1.qry");
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
                Qeury_struct[index_qry].iswhat = value_qry.c_str();
            }
            if (k == 1) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].min = num_float;
            }
            if (k == 2) {
                num_float = stof(value_qry);
                Qeury_struct[index_qry].max = num_float;
            }
            k++;
        }
        index_qry++;
    }
    fin_qry.close();

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    쿼리 구조체 배열에다가 값 넣어주기 ( 나중에 꺼내서 쓰기 위함 )
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

    vector <int> result_answer;

    for(int i=1; i<=size_qry; i++){
        if(Qeury_struct[i].iswhat == "GPA"){
            //cout << "GPA, " << i << endl;
            search(GPA_tree, Qeury_struct[i].min, Qeury_struct[i].max);
            result_answer.push_back(search_answer);
            search_answer = 0;
        }

        if(Qeury_struct[i].iswhat == "Weight"){
            //cout << "Weight, " << i << endl;
            search(weight_tree, Qeury_struct[i].min, Qeury_struct[i].max);
            result_answer.push_back(search_answer);
            search_answer = 0;
        }

        if(Qeury_struct[i].iswhat == "Height"){
            //cout << "Height, " << i << endl;
            search(height_tree, Qeury_struct[i].min, Qeury_struct[i].max);
            result_answer.push_back(search_answer);
            search_answer = 0;
        }
    }

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    search 결과 result 벡터에 넣어주기 ( 결과값 )
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
    cout <<5;
    ofstream fout_out("students1.out");

    for(int i=0; i<result_answer.size(); i++) {
        fout_out << result_answer[i] << "\n";
    }

    /*cout << "answer" << endl;
    for(int i=1; i<=result_answer.size(); i++){
        cout << result_answer[i] << endl;
    }*/

    fout_out.close();

    //system("pause");

    return 0;
}
