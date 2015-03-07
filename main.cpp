/* 
 * File:   main.cpp
 * Author: Toshiki Arita
 *      
 *      
 *      Schema file was modified to not have a \n character on "REFERENCES". 
 * 
 * Created on January 29, 2014, 9:33 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <sstream>

using namespace std;

/*
 * 
 */

string compPrimKey(string str, string name) {
    //cout << "( " << str.find('(') << " , " << str.find(',') << endl;

    return "[label=\"" + name + "\"shape=\"box\" style=\"rounded\"width=0 height=0 color=\"#00AA00\"fontname=\"Courier-Bold\"]\n";

}

string formatTableName(string str) {
    return str + "[shape=\"box3d\" style=\"filled\" color=\"#0000FF\" fillcolor=\"#EEEEEE\" fontname=\"Courier\"] \n";
}

string formatAttribute(string str, int pick) {
    // pick = 0 if var char
    // pick = 1 if numeric
    // pick = 2 if single primary key
    // pick = 3 if primary & foreign key
    string finished;


    if (pick != 1)
        finished = "[label=\"" + str.substr(1, str.find("VARCHAR") - 1) + "\"shape=\"box\" style=\"rounded\"width=0 height=0 ";
    else if (pick == 1)
        finished = "[label=\"" + str.substr(1, str.find("NUMERIC") - 1) + "\"shape=\"box\" style=\"rounded\"width=0 height=0 ";

    if (str.find("PRIMARY KEY") != -1 && pick == 2)
        return finished + "color=\"#00AA00\"fontname=\"Courier-Bold\"]\n";
    else if (pick == 3 && str.find("PRIMARY KEY") != -1)
        return finished + "color=\"#0000FF\"fontname=\"Courier-Bold\"]\n";
    else if (pick == 3)
        return finished + "color=\"#0000FF\"]\n";
    else
        return finished + "color=\"#00AA00\"]\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage : PSV <Schema File> " << endl;
        return 1;
    }
    // open file for parsing
    fstream inFile;
    inFile.open(argv[1]);
    // open & create new file for printing
    ofstream outFile;
    outFile.open("view.dot", ofstream::trunc);

    // start graph
    cout << "digraph { \n rankdir = BT \n ";

    if (inFile.is_open() && outFile.is_open()) {
        string inString;
        string tableName;
        string temp;
        while (inFile.good()) { // first pass to create nodes & attribute edges only
            getline(inFile, inString, '\n'); // parsing line by line

            //table
            if (inString.find("CREATE TABLE") != -1) {
                tableName = inString.substr(13);
                cout << formatTableName(tableName);
            }

            //attributes - varchar & single primary key 

            if (inString.find("VARCHAR") != -1) { // pick = 0
                temp = inString.substr(1, inString.find("VARCHAR") - 1);
                // single primary key without reference
                if (inString.find("PRIMARY KEY") != -1) {
                    cout << tableName << "_" << temp << formatAttribute(inString, 2);
                    cout << tableName << "->" << tableName << "_" << temp << "[dir=\"none\"]" << endl;
                }// regular attribute
                else {
                    cout << tableName << "_" << temp << formatAttribute(inString, 0);
                    cout << tableName << "->" << tableName << "_" << temp << "[dir=\"none\"]" << endl;
                }

            }
            //attributes - numeric
            if (inString.find("NUMERIC") != -1) { // pick = 1
                temp = inString.substr(1, inString.find("NUMERIC") - 1);
                cout << tableName << "_" << temp << formatAttribute(inString, 1);
                cout << tableName << "->" << tableName << "_" << temp << "[dir=\"none\"]" << endl;
            }

            // composite or compound primary key
            if (inString.find("PRIMARY KEY (") != -1) {
                temp = inString.substr(inString.find('(') + 1, inString.find(',') - inString.find('(') - 1);
                cout << tableName << "_" << temp << compPrimKey(inString, temp);
            }

        } //while not eof

        inFile.clear();                 // clear flags
        inFile.seekg(0, ios_base::beg); // reset file position to start

        while (inFile.good()) { // take care of foreign keys
            string foreign;
            getline(inFile, inString, '\n'); // parsing line by line

            // get table name
            if (inString.find("ALTER TABLE") != -1)
                tableName = inString.substr(12);
            else if (inString.find("CREATE TABLE") != -1)
                tableName = inString.substr(13);

            // primary & foreign key
            if (inString.find("VARCHAR") != -1 && inString.find("REFERENCES") != -1) { //prime & foreign key
                temp = inString.substr(1, inString.find("VARCHAR") - 1);
                foreign = inString.substr(inString.find("REFERENCES") + 11, inString.find(" DEFERRABLE") - inString.find("REFERENCES") - 10);
                cout << tableName << "_" << temp << formatAttribute(inString, 3); // change box color
                cout << tableName << "_" << temp << "->" << foreign << "[color=red dir=foward style=dashed label=\" FK\" fontname=\"Verdana\" fontcolor=red fontsize=10]\n";
            }

            // alter table foreign key
            if (inString.find("FOREIGN KEY") != -1) {

                // get nodes
                string temp2;
                temp = inString.substr(inString.find('(') + 1, inString.find(')') - inString.find('(') - 1);
                temp2 = inString.substr(inString.find('(', inString.find('(') + 1) + 1, inString.find(')', inString.find(')') + 1) - inString.find('(', inString.find('(') + 1) - 1);

                //get ready for parsing
                istringstream is(temp);
                istringstream iss(temp2);
                string token, tokenTwo, ref;

                //get foreign key node
                ref = inString.substr(inString.find("REFERENCES") + 11, inString.find('(', inString.find('(') + 1) - inString.find("REFERENCES") - 11);

                while (iss.good()) {
                    getline(is, token, ',');
                    getline(iss, tokenTwo, ',');

                    cout << tableName << "_";

                    if (tokenTwo[0] == ' ')
                        cout << token.substr(1, string::npos);
                    else
                        cout << token;

                    cout << "->" << ref << "_";


                    if (tokenTwo[0] == ' ')
                        cout << tokenTwo.substr(1, string::npos);
                    else
                        cout << tokenTwo;

                    cout << "[color=red dir=foward style=dashed label=\" FK\" fontname=\"Verdana\" fontcolor=red fontsize=10]\n";
                }


            }
        }// while not eof
        cout << "}";
        inFile.close(); // close files because we're finished
        outFile.close();
    } // if infile is open

    return 0;
}

