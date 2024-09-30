# Herald
![Build Status](https://github.com/Justin-Randall/herald/actions/workflows/cmake-multi-platform.yml/badge.svg) [![codecov](https://codecov.io/github/Justin-Randall/herald/graph/badge.svg?token=4HYUF98OJ4)](https://codecov.io/github/Justin-Randall/herald)


A multi-purpose C++ logging library.

Log() -> Transformer() -> Writer()

For example: 

```cpp
Log(Level, Message, Key1, Value1, Key2, Value2 ...) -> JsonTransformer() -> FileWriter()
```

or (not yet implemented)

```cpp
Log(Level, Message, Key1, Value1, Key2, Value2 ...) -> JsonTransformer() -> ElasticSearchWriter()
```

## Usage:

Create a Writer, give it to a transformer:

```cpp
auto fileLogBuilder = Herald::createFileWriterBuilder();
auto fileLogWriter = fileLogBuilder->addConfigPair("filename", "stdout").build();

auto simpleBuilder = Herald::createSimpleLogTransformerBuilder();
auto simpleTransformer = simpleBuilder->attachLogWriter(fileLogWriter).build();

// ... use it anywhere the transformer is handy
Herald::log(simpleTransformer, LogLevels::Info, "Informational log message");

// ... set headers to include for each log
simpleTransformer->addHeader("userId", userId);
simpleTransformer->addHeader("machineId", hostName);

Herald::log(simpleTransformer, LogLevels::Debug, "Something happening");
// output will include userId=<userId> machineId=<hostName>
// or if a JsonTransformer is used, the headers will be listed as part of the well-formed json, suitable for ingestion by endpoints like ElasticSearch
```
