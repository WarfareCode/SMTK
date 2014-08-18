#include "smtk/remote/RemusBridgeConnections.h"

#include "smtk/remote/RemusBridgeConnection.h"

#include "remus/client/ServerConnection.h"

#include <map>

namespace smtk {
  namespace model {

//typedef smtk::shared_ptr<remus::client::ServerConnection> ClientConnectionType;
typedef smtk::shared_ptr<smtk::model::RemusBridgeConnection> ClientConnectionType;
//typedef std::vector<ClientConnectionType> ConnectionArrayType;

struct RemusServerDetails
{
  ClientConnectionType connection;
};

typedef std::map<std::string,RemusServerDetails> ConnectionMapType;

class RemusBridgeConnections::Internal
{
public:
  ConnectionMapType connections;
};

RemusBridgeConnections::RemusBridgeConnections()
{
  this->m_data = new Internal;
}

RemusBridgeConnections::~RemusBridgeConnections()
{
  delete this->m_data;
}

/**\brief Connect to an SMTK model server at the given \a host and \a port.
  *
  * The \a host string should either be empty or a hostname or an IP address.
  * An empty hostname indicates that a local, in-process server should be used.
  *
  * The \a  port is an optional integer. If negative, the default port will be used.
  */
RemusBridgeConnection::Ptr RemusBridgeConnections::connectToServer(const std::string& host, int port)
{
  std::string url;
  if (!host.empty())
    {
    std::ostringstream urlSS;
    urlSS << host;
    if (port > 0)
     urlSS << ":" << port;
    url = urlSS.str();
    }

  ConnectionMapType::iterator it = this->m_data->connections.find(url);
  if (it != this->m_data->connections.end())
    return it->second.connection;

  RemusBridgeConnection::Ptr conn = RemusBridgeConnection::create();
  if (!url.empty())
    conn->connectToServer(host, port);
  this->m_data->connections[url].connection = conn;

  // TODO: ask server for supported worker types then ask each worker for
  //       its list of bridges and their file types. Finally, register
  //       each bridge type under an alias.
  return conn;
}

  } // namespace model
} // namespace smtk
