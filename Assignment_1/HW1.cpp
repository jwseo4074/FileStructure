#include <iostream>
#include <fstream>
#include <sstream>
//#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
using namespace std;



struct Student {
    char name[80];
    char ID[10];
    float GPA;
    float weight;
    float height;
    int age;

    // 구조체 하나당 할당된 메모리 -> 106
    // 블럭 한개가 4096 이므로 컴팩트하게 38개의 구조체가 한 블럭에 들어감 !!
    // 4028 바이트 (구조체 한개) + 68 바이트 => 4096 컴팩트 하게 들어감.
};

bool compare_name(const Student& name1, const Student& name2) {
    if (name1.name < name2.name)
        return true;
    else
        return false;
}

bool compare_GPA(const Student& gpa1, const Student& gpa2) {
    if (gpa1.GPA > gpa2.GPA)
        return true;
    else
        return false;
}

struct Student Student_struct[1060][38];
vector <Student> Student_vec;

int row_index = 0;
int col_index = 0;

int main(void) {
    ifstream fin("students.inp");
    ofstream fout("students.bin", ios::out | ios::binary);
    ofstream fout_name("students-name.bin", ios::out | ios::binary);
    ofstream fout_GPA("students-GPA.bin", ios::out | ios::binary);

    int size;
    string str;
    //int file_size = fin.tellg();
    fin >> size;
    //cout << "size = " << size << endl;
    float num_float;
    int num_int;
    vector <string> result;

    while (!fin.eof()) {
        getline(fin, str);     // 구조체 하나 읽어오기.
        string token;
        stringstream ss(str);
        int k = 0;
        string value;
        while (getline(ss, token, ',')) {     // 구조체 한개 안에서 값들 6개 차례대로 읽어오기
            result.push_back(token);
            value = *(result.end() - 1);
            if (k == 0) {
                const char* c = value.c_str();
                strcpy(Student_struct[row_index][col_index].name,c);
                //cout << "name = " << Student_struct[row_index][col_index].name << endl;
            }
            else if (k == 1) {
                const char* c = value.c_str();
                strcpy(Student_struct[row_index][col_index].ID, c);
                //cout << "ID = " << Student_struct[row_index][col_index].ID << endl;
            }
            else if (k == 2) {
                num_float = stof(value);
                Student_struct[row_index][col_index].GPA = num_float;
                //cout << "GPA = " << Student_struct[row_index][col_index].GPA << endl;
            }
            else if (k == 3) {
                num_float = stof(value);
                Student_struct[row_index][col_index].weight = num_float;
                //cout << "weight" << Student_struct[row_index][col_index].weight << endl;
            }
            else if (k == 4) {
                num_float = stof(value);
                Student_struct[row_index][col_index].height = num_float;
                //cout << "height = " << Student_struct[row_index][col_index].height << endl;
            }
            else if (k == 5) {
                num_int = stoi(value);
                Student_struct[row_index][col_index].age = num_int;
                //cout <<"age = " << Student_struct[row_index][col_index].age << endl;
            }
            k++;
        }

        // 값이 잘 들어갔는지 확인하기
        /*
        cout << "\n\n\n";
        cout << "  Student_struct[" << row_index << "][" << col_index << "].name  = " << Student_struct[row_index][col_index].name << endl;
        cout << "  Student_struct[" << row_index << "][" << col_index << "].ID  = " << Student_struct[row_index][col_index].ID << endl;
        cout << "  Student_struct[" << row_index << "][" << col_index << "].GPA  = " << Student_struct[row_index][col_index].GPA << endl;
        cout << "  Student_struct[" << row_index << "][" << col_index << "].height  = " << Student_struct[row_index][col_index].height << endl;
        cout << "  Student_struct[" << row_index << "][" << col_index << "].weight  = " << Student_struct[row_index][col_index].weight << endl;
        cout << "  Student_struct[" << row_index << "][" << col_index << "].age  = " << Student_struct[row_index][col_index].age << endl;
        */

        col_index++;        // 한칸 오른쪽으로
        if (col_index == 38) {    // row 32개 꽉차면 한칸 밑으로
            row_index++;
            col_index = 0;
        }
        result.clear();      // ex >  구조체 하나 => [0][1]  , 꽉찼으니 벡터 초기화
    }
    // 읽고 저장 끝.

    // 구조체 벡터에다가 푸쉬백 => 4만개의 구조체가 들어있는 벡터   => sort 할때 사용
    for (int i = 0; i < 1060; i++) {
        for (int j = 0; j < 38; j++) {
            Student_vec.push_back(Student_struct[i][j]);
            //cout << " i - " << i << "j - " << j  << endl; //확인
        }
    }

    //ibg 64바이트
    char c[64] = { 0, };

    // 바이너리 파일로 출력 !!   <1번>
    for (int i = 0; i < 1060; i++) {
        for (int j = 0; j < 38; j++) {
            fout.write((char*)&Student_struct[i][j].height, sizeof(Student_struct[i][j].name));
            fout.write((char*)&Student_struct[i][j].height, sizeof(Student_struct[i][j].ID));
            fout.write((char*)&Student_struct[i][j].height, sizeof(Student_struct[i][j].GPA));
            fout.write((char*)&Student_struct[i][j].height, sizeof(Student_struct[i][j].weight));
            fout.write((char*)&Student_struct[i][j].height, sizeof(Student_struct[i][j].height));
            fout.write((char*)&Student_struct[i][j].height, sizeof(Student_struct[i][j].age));
            fout.write((char*)&c, sizeof(c));   // ibg 68
            // 이로써 파일 저장할때는 4028 바이트 (구조체 한개) + 68 바이트 => 4096 컴팩트 하게 들어감.
        }
    }       //바이너리로 저장


    sort(Student_vec.begin(), Student_vec.end(), compare_name);   //compare_name 을 이용해서 sort ( name 오름차순 )
    for (int k = 0; k < size; k++)
        fout_name.write((char*)&Student_vec[k], sizeof(Student_vec[k]));

    sort(Student_vec.begin(), Student_vec.end(), compare_GPA);     //compare_GPA 를 이용해서 sort ( GPA 내림차순 )
    for (int k = 0; k < size; k++)
        fout_GPA.write((char*)&Student_vec[k], sizeof(Student_vec[k]));

    fin.close();
    fout.close();


    //3번 과제
    //students-name 파일 읽고 나서 search
    ifstream f3in("students-name.inp", ios::binary);       // 바이너리로 읽어오기
    ifstream f3qry("students.qry");
    ofstream f3out("students.out");           // 바이너리 -> text (ASKII) 로 전환


    /*< 구조체 배열에서 scanning name >

    sol >
        그냥 검색하면 linear scanning 이니까
        <<<< "블럭 단위로" 가져와서 보기 => 로우 하나가 4K !! (4096) 딱 맞춰놓음
        ( 1 번에서 데이터 받고 write 할때 구현해놓음 , 블럭단위 ) >>>>>
    */



    f3in.close();
    f3qry.close();
    f3out.close();

    return 0;
}
