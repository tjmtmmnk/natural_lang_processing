#ifndef NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H
#define NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H

void setFileName(char *_file_name);
void initScan();
void openFile();
void closeFile();

// @return : 次のトークンコード
int scanTokenOneEach();

int getLatestFoundTokenLine();




#endif //NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H
