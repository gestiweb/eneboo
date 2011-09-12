// MsspGwCheck.h
#ifndef __MSSP_ERRORS__
#define __MSSP_ERRORS__

#define REQUEST_OK 100
#define REQUEST_OK_DESC "MSSP Request OK"
#define REQUEST_OK_CODE "REQUEST_OK"
#define WRONG_PARAM 101
#define WRONG_PARAM_DESC "Error among the arguments of the request"
#define MISSING_PARAM 102
#define MISSING_PARAM_DESC "An argument in the request is missing"
#define WRONG_DATA_LENGTH 103
#define WRONG_DATA_LENGTH_DESC "The DataToBeSigned are too large. Limitations are due to the Mobile Signature technology implemented by the MSSP"
#define UNAUTHORIZED_ACCESS 104
#define UNAUTHORIZED_ACCESS_DESC "The AP is unknown or the password is wrong or the AP asks for an additional service for which it has not subscribed"
#define UNKNOWN_CLIENT 105
#define UNKNOWN_CLIENT_DESC "The enduser targeted by the AP is unknown from the MSSP"
#define HANDSHAKE_REQUIRED 106
#define HANDSHAKE_REQUIRED_DESC "The MSSP wants prior to negotiate with the AP the use of XML signatures in the messages"
#define INAPPROPRIATE_DATA 107
#define INAPPROPRIATE_DATA_DESC "MSSP cannot handle given MIME Type or encoding style of the DataToBeSigned or DataToBeDisplayed"
#define INCOMPATIBLE_INTERFACE 108
#define INCOMPATIBLE_INTERFACE_DESC "the minor version and/or major version parameters are inappropriate for the receiver of the message"
#define UNSUPPORTED_PROFILE 109
#define UNSUPPORTED_PROFILE_DESC "The AP has specified a Mobile Signature Profile that the MSSP does not support"
#define EXPIRED_TRANSACTION 208
#define EXPIRED_TRANSACTION_DESC "Transaction Expiry date has been reached or Time out has elapsed"
#define EXPIRED_TRANSACTION_CODE "EXPIRED_TRANSACTION"
#define OTA_ERROR 209
#define OTA_ERROR_DESC "The MSSP has not succeeded to contact the enduser's mobile equipment (Bad connection…)"
#define USER_CANCEL 401
#define USER_CANCEL_CODE "USER_CANCEL"
#define USER_CANCEL_DESC "The client has cancelled the transaction"
#define PIN_NR_BLOCKED 402
#define PIN_NR_BLOCKED_DESC "PIN-NR is blocked"
#define CARD_BLOCKED 403
#define CARD_BLOCKED_DESC "Card is blocked"
#define NO_KEY_FOUND 404
#define NO_KEY_FOUND_DESC "No key found!"
#define NO_URL_FOUND 405
#define NO_URL_FOUND_DESC "No URL found"
#define PB_SIGNATURE_PROCESS 406
#define PB_SIGNATURE_PROCESS_DESC "Error during the Mobile Signature process on the Mobile equipment"
#define REGISTRATION_NOK 407
#define REGISTRATION_NOK_DESC "Registration not OK"
#define NO_CERT_FOUND 422
#define NO_CERT_FOUND_DESC "No certificate has been found for this msisdn"
#define CRL_PB 423
#define CRL_PB_DESC "CRL PB"
#define CRL_EXPIRED 424
#define CRL_EXPIRED_DESC "CRL expired"
#define ERROR_CERTIFICATE 425
#define ERROR_CERTIFICATE_DESC "Error during the Certificate verification. The platform does not assume if the certificate is revoked or not"
#define INTERNAL_ERROR 900
#define INTERNAL_ERROR_CODE "INTERNAL_ERROR"
#define INTERNAL_ERROR_DESC "Unknown Error"

#define USER_SIGN  400 
#define USER_SIGN_CODE "USER_SIGN"
#define USER_SIGN_DESC "The enduser has received the Mobile Signature request on his Mobile equipment"
#define REGISTRATION_OK 408 
#define REGISTRATION_OK_CODE "REGISTRATION_OK"
#define REGISTRATION_OK_DESC ""
#define SIGNATURE 500 
#define SIGNATURE_CODE "SIGNATURE"
#define SIGNATURE_DESC "A Mobile Signature has been successfully constructed and is available"
#define REVOKED_CERTIFICATE 501 
#define REVOKED_CERTIFICATE_CODE "REVOKED_CERTIFICATE"
#define REVOKED_CERTIFICATE_DESC "A Mobile Signature has been successfully constructed. But, the signer's certificate is revoked"
#define VALID_SIGNATURE 502 
#define VALID_SIGNATURE_CODE "VALID_SIGNATURE"
#define VALID_SIGNATURE_DESC "A Mobile Signature has been successfully constructed and the signature is valid"
#define INVALID_SIGNATURE 503 
#define INVALID_SIGNATURE_CODE "INVALID_SIGNATURE"
#define INVALID_SIGNATURE_DESC "An XML Signature has been successfully constructed and the signature is not valid"
#define SIGNATURE_INVALID_DESC "Received signature was not a valid RSA-SHA1 signature"
#define OUTSTANDING_TRANSACTION 504 
#define OUTSTANDING_TRANSACTION_CODE "OUTSTANDING_TRANSACTION"
#define OUTSTANDING_TRANSACTION_DESC "Transaction is not complete. Must call again later"
#define RET_OK_PUSH 600 
#define RET_OK_PUSH_DESC "OK with PUSH confirmation The message has been received and is going to be processed. A confirmation must be sent to the enduser (text SMS)"
#define RET_OK_NOPUSH 601
#define RET_OK_NOPUSH_DESC "OK without PUSH confirmation The message has been received and is going to be processed. No confirmation to the enduser"
#define RET_NOK_PUSH 602
#define RET_NOK_PUSH_DESC "NOK with PUSH Information Problem when receiving the message. Information must be sent to the enduser"
#define RET_NOK_NOPUSH 603
#define RET_NOK_NOPUSH_DESC "NOK without PUSH information Problem when receiving the message. No Information sent to the enduser"


#endif   // __MSSP_ERRORS__
