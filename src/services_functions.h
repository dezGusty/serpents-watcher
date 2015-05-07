#pragma once


namespace serpents
{
  namespace services
  {
    typedef enum 
    {
      SvcStatusStopped = 0,
      SvcStatusRunning = 1,
      SvcStatusError = 2
    } Status;

    //
    // Start a (Windows) service identified by a name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered with, not the display name of the service.
    // @return true if the service is running (either already running or running at the end of this call).
    //
    // @authors Petru Barko, Augustin Preda.
    //
    bool StartServiceWithName(const char* service_name);

    //
    // Stop a (Windows) service identified by a name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered with, not the display name of the service.
    // @return true if the service is stopped (either already stopped or stopped at the end of this call).
    //
    // @authors Petru Barko, Augustin Preda.
    //
    bool StopServiceWithName(const char* service_name);

    //
    // Get the status of a service, identified by its name.
    // @param service_name The name of the service to use.
    // Please keep in mind that this is the name that a service is registered with, not the display name of the service.
    // @return The found status.
    //
    // @authors Petru Barko, Augustin Preda.
    //
    Status GetServiceStatus(const char* service_name);
  }
}