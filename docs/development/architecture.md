# Architecture

The most important tenet of Cppinecone's architecture is that using the library must be simple for end users. Users
generally do not care about details of how the client works internally, nor should the library prescribe specific
behaviors or methodologies on our users whenever possible. With this in mind, the most important concept in the
architecture is:

* The public client API

The client API is in turn driven by a small number of "top-level" concepts that allow for easy implementation of
communication with Pinecone's REST API:

* URL construction
* HTTP communication

Finally, we must encode the data structures and communication protocols for each operation that the Pinecone API
supports. This is provided by:

* Individual API operations

Each of these concepts is briefly explained here. The purpose of this document is to provide a high-level overview of
_why_ the code is structured in this way; it does not attempt to explain _how_ each individual component works or
why low-level implementation details were chosen.

## The public client API

## URL construction

## HTTP communication

## Individual API operations