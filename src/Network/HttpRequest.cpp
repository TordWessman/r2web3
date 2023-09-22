#include "HttpRequest.h"
#include "HttpResponse.h"
namespace blockchain
{
    HttpResponse HttpRequest::SendRequest(const char *url) const
    {
        return network->MakeRequest(url, httpMethod, requestBody);
    }
}
