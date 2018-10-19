#ifndef NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H
#define NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H

#define MAX_WORD_LENGTH 1000
void initScan();
void setFileName(char *_file_name);
void closeFile();

// @return : 次のトークンコード
int scanTokenOneEach();

int getLatestFoundTokenLine();




#endif //NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H
