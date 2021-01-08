/*
 * TroopTreasurer
 * Author: Karl Moad
 * ---------------------------------
 * Object Error - General error class with
 * error code identifier to allow for simplified
 * code handling
 * Emitted by all object lib classes.
*/

#ifndef TROOPTREASURER_OBJECTERROR_H
#define TROOPTREASURER_OBJECTERROR_H

#include <QException>

enum class ObjectErrorCode
{
    UNKNOWN_ERROR = 0,
    ERROR_READ_FILE = 1000,
    ERROR_WRITE_FILE = 1100,
    ERROR_NO_FILE = 1200,
    EXPRESSION_PARSING=2000,
    GENERAL_OBJECT_ERROR = 9999
};

class ObjectError : public QException
{
public:
    ObjectError(QString const& text="", int code=-1) noexcept;
    ObjectError(const ObjectError &re);
    virtual ~ObjectError() override;

    void raise();
    ObjectError *clone() const override;
    const char *what() const noexcept override;
    int errorCode() const;

private:
    QString _message;
    int _errorCode;
};





#endif //TROOPTREASURER_OBJECTERROR_H
