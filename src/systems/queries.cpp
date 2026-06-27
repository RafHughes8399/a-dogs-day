#include "queries.h"

queries::query_executor<bool> queries::bool_executor_;
queries::query_executor<int> queries::int_executor_;
queries::query_executor<std::vector<Vector2>> queries::path_executor_;
