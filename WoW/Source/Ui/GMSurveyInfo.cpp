#include "GMSurveyInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F762F4/0x140E6C2F0
uint8_t CGGMSurveyInfo::m_surveyAnswers[15];

// STATIC: 0x00F7630C/0x140E6C310
char* CGGMSurveyInfo::m_surveyAnswerComments[15];

// STATIC: 0x00F76348/0x140E6C308
char* CGGMSurveyInfo::m_surveyComment;

// OFFSET: 0x009E5C00/0x14077E060
const char* CGGMSurveyInfo::GetQuestion(int32_t index) {
    if (index < 1 || static_cast<uint32_t>(index) > 15) {
        return NULL;
    }

    GMSurveyCurrentSurveyRec* current = g_gmSurveyCurrentSurveyDB.GetRecord(s_locale);
    GMSurveySurveysRec* survey = g_gmSurveySurveysDB.GetRecord(!current ? 0 : current->m_gmsurveyID);

    if (!survey) {
        return NULL;
    }

    int32_t questionId = survey->m_questionID[index - 1];

    if (questionId <= 0) {
        return NULL;
    }

    GMSurveyQuestionsRec* question = g_gmSurveyQuestionsDB.GetRecord(questionId);
    return question ? static_cast<const char*>(question->m_question) : NULL;
}

// OFFSET: 0x009E5C80/0x14077E120
const char* CGGMSurveyInfo::GetAnswer(int32_t index, int32_t answer) {
    if (index < 1 || static_cast<uint32_t>(index) > 15) {
        return NULL;
    }

    GMSurveyCurrentSurveyRec* current = g_gmSurveyCurrentSurveyDB.GetRecord(s_locale);
    GMSurveySurveysRec* survey = g_gmSurveySurveysDB.GetRecord(current ? current->m_gmsurveyID : 0);

    if (!survey) {
        return NULL;
    }

    int32_t questionId = survey->m_questionID[index - 1];

    if (questionId <= 0 || !g_gmSurveyQuestionsDB.GetRecord(questionId)) {
        return NULL;
    }

    for (int32_t i = 0; i < g_gmSurveyAnswersDB.m_numRecords; i++) {
        GMSurveyAnswersRec* rec = g_gmSurveyAnswersDB.GetRecordByIndex(i);

        if (rec && rec->m_gmsurveyQuestionID == questionId && rec->m_sortIndex == answer - 1) {
            return rec->m_answer;
        }
    }

    return NULL;
}

// OFFSET: 0x009E5D50/0x14077E230
int32_t CGGMSurveyInfo::GetNumAnswers(int32_t index) {
    if (index < 1 || static_cast<uint32_t>(index) > 15) {
        return 0;
    }

    GMSurveyCurrentSurveyRec* current = g_gmSurveyCurrentSurveyDB.GetRecord(s_locale);

    if (!g_gmSurveySurveysDB.GetRecord(current ? current->m_gmsurveyID : 0)) {
        return 0;
    }

    if (index - 1 <= 0 || !g_gmSurveyQuestionsDB.GetRecord(index - 1)) {
        return 0;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < g_gmSurveyAnswersDB.m_numRecords; i++) {
        GMSurveyAnswersRec* rec = g_gmSurveyAnswersDB.GetRecord(i);

        if (rec && rec->m_gmsurveyQuestionID == index - 1) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x009E64A0/0x14077EB00
void CGGMSurveyInfo::Submit() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_SURVEY_SUBMIT));
    GMSurveyCurrentSurveyRec* current = g_gmSurveyCurrentSurveyDB.GetRecord(s_locale);
    msg.Put(static_cast<uint32_t>(current ? current->m_gmsurveyID : 0));

    for (uint32_t i = 0; i < 15; i++) {
        GMSurveyCurrentSurveyRec* currentSurvey = g_gmSurveyCurrentSurveyDB.GetRecord(s_locale);
        GMSurveySurveysRec* survey = g_gmSurveySurveysDB.GetRecord(currentSurvey ? currentSurvey->m_gmsurveyID : 0);
        int32_t questionId = survey ? survey->m_questionID[i] : 0;
        msg.Put(static_cast<uint32_t>(questionId));

        if (!questionId) {
            break;
        }

        msg.Put(CGGMSurveyInfo::m_surveyAnswers[i]);
        msg.PutString(CGGMSurveyInfo::m_surveyAnswerComments[i] ? CGGMSurveyInfo::m_surveyAnswerComments[i] : "");
    }

    msg.PutString(CGGMSurveyInfo::m_surveyComment ? CGGMSurveyInfo::m_surveyComment : "");
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGMSurveyInfo::FreeAnswers();
}

// OFFSET: 0x00401000/0x1407D7E80
void CGGMSurveyInfo::InitializeGame() {
}

// OFFSET: 0x009E57B0/0x14077DB10
void CGGMSurveyInfo::FreeAnswers() {
    for (uint32_t i = 0; i < 15; i++) {
        CGGMSurveyInfo::m_surveyAnswers[i] = 0;

        if (CGGMSurveyInfo::m_surveyAnswerComments[i]) {
            SMemFree(CGGMSurveyInfo::m_surveyAnswerComments[i], __FILE__, __LINE__, 0);
        }

        CGGMSurveyInfo::m_surveyAnswerComments[i] = NULL;
    }

    if (CGGMSurveyInfo::m_surveyComment) {
        SMemFree(CGGMSurveyInfo::m_surveyComment, __FILE__, __LINE__, 0);
    }

    CGGMSurveyInfo::m_surveyComment = NULL;
}

// OFFSET: 0x009E5820/0x14077DBB0
void CGGMSurveyInfo::SetAnswer(uint32_t index, uint8_t answer, const char* comment) {
    if (static_cast<int32_t>(index) < 0 || index >= 15) {
        return;
    }

    CGGMSurveyInfo::m_surveyAnswers[index] = answer;

    if (CGGMSurveyInfo::m_surveyAnswerComments[index]) {
        SMemFree(CGGMSurveyInfo::m_surveyAnswerComments[index], __FILE__, __LINE__, 0);
    }

    CGGMSurveyInfo::m_surveyAnswerComments[index] = NULL;

    if (comment && *comment) {
        CGGMSurveyInfo::m_surveyAnswerComments[index] = SStrDupA(comment, __FILE__, __LINE__);
    }
}

// OFFSET: 0x009E5890/0x14077DC50
void CGGMSurveyInfo::SetComment(const char* comment) {
    if (CGGMSurveyInfo::m_surveyComment) {
        SMemFree(CGGMSurveyInfo::m_surveyComment, __FILE__, __LINE__, 0);
    }

    CGGMSurveyInfo::m_surveyComment = NULL;

    if (comment && *comment) {
        CGGMSurveyInfo::m_surveyComment = SStrDupA(comment, __FILE__, __LINE__);
    }
}

// OFFSET: 0x009E5BF0/0x14077E050
void CGGMSurveyInfo::ShutdownGame() {
    CGGMSurveyInfo::FreeAnswers();
}
