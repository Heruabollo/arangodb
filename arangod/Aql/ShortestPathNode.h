////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_AQL_SHORTEST_PATH_NODE_H
#define ARANGOD_AQL_SHORTEST_PATH_NODE_H 1

#include "Aql/GraphNode.h"
#include "VocBase/TraverserOptions.h"

#include <velocypack/Builder.h>

namespace arangodb {
namespace aql {

/// @brief class ShortestPathNode
class ShortestPathNode : public GraphNode {
  friend class ExecutionBlock;
  friend class RedundantCalculationsReplacer;
  friend class ShortestPathBlock;

  /// @brief constructor with a vocbase and a collection name
 public:
  ShortestPathNode(ExecutionPlan* plan, size_t id, TRI_vocbase_t* vocbase,
                AstNode const* direction, AstNode const* start, AstNode const* target,
                AstNode const* graph, traverser::ShortestPathOptions* options);

  ShortestPathNode(ExecutionPlan* plan, arangodb::velocypack::Slice const& base);

  ~ShortestPathNode() {}

  /// @brief Internal constructor to clone the node.
 private:
  ShortestPathNode(
      ExecutionPlan* plan, size_t id, TRI_vocbase_t* vocbase,
      std::vector<std::unique_ptr<aql::Collection>> const& edgeColls,
      std::vector<std::unique_ptr<aql::Collection>> const& reverseEdgeColls,
      std::vector<std::unique_ptr<aql::Collection>> const& vertexColls,
      std::vector<TRI_edge_direction_e> const& directions,
      Variable const* inStartVariable, std::string const& startVertexId,
      Variable const* inTargetVariable, std::string const& targetVertexId,
      traverser::ShortestPathOptions* options);

 public:
  /// @brief return the type of the node
  NodeType getType() const override final { return SHORTEST_PATH; }

  /// @brief export to VelocyPack
  void toVelocyPackHelper(arangodb::velocypack::Builder&,
                          bool) const override final;

  /// @brief clone ExecutionNode recursively
  ExecutionNode* clone(ExecutionPlan* plan, bool withDependencies,
                       bool withProperties) const override final;

  /// @brief the cost of a traversal node
  double estimateCost(size_t&) const override final;

  /// @brief Test if this node uses an in variable or constant for start
  bool usesStartInVariable() const {
    return _inStartVariable != nullptr;
  }

  /// @brief return the start variable
  Variable const* startInVariable() const { return _inStartVariable; }

  std::string const getStartVertex() const { return _startVertexId; }

  /// @brief Test if this node uses an in variable or constant for target
  bool usesTargetInVariable() const {
    return _inTargetVariable != nullptr;
  }

  /// @brief return the target variable
  Variable const* targetInVariable() const { return _inTargetVariable; }

  std::string const getTargetVertex() const { return _targetVertexId; }

  /// @brief getVariablesSetHere
  std::vector<Variable const*> getVariablesSetHere() const override final {
    std::vector<Variable const*> vars;
    if (_vertexOutVariable != nullptr) {
      vars.emplace_back(_vertexOutVariable);
    }
    if (_edgeOutVariable != nullptr) {
      vars.emplace_back(_edgeOutVariable);
    }
    return vars;
  }

  /// @brief getVariablesUsedHere, returning a vector
  std::vector<Variable const*> getVariablesUsedHere() const override {
    std::vector<Variable const*> vars;
    if (_inStartVariable != nullptr) {
      vars.emplace_back(_inStartVariable);
    }
    if (_inTargetVariable != nullptr) {
      vars.emplace_back(_inTargetVariable);
    }
    return vars;
  }

  /// @brief getVariablesUsedHere, modifying the set in-place
  void getVariablesUsedHere(
      std::unordered_set<Variable const*>& vars) const override {
    if (_inStartVariable != nullptr) {
      vars.emplace(_inStartVariable);
    }
    if (_inTargetVariable != nullptr) {
      vars.emplace(_inTargetVariable);
    }
  }

  traverser::ShortestPathOptions* options() const override;

  /// @brief Compute the traversal options containing the expressions
  ///        MUST! be called after optimization and before creation
  ///        of blocks.
  void prepareOptions() override;

 private:

  /// @brief Helper function to parse all collection names / directions
  void addEdgeColl(std::string const& name, TRI_edge_direction_e dir) override;

  /// @brief input variable only used if _vertexId is unused
  Variable const* _inStartVariable;

  /// @brief input vertexId only used if _inVariable is unused
  std::string _startVertexId;

  /// @brief input variable only used if _vertexId is unused
  Variable const* _inTargetVariable;

  /// @brief input vertexId only used if _inVariable is unused
  std::string _targetVertexId;

  /// @brief the reverse edge collections
  std::vector<std::unique_ptr<aql::Collection>> _reverseEdgeColls;

};

} // namespace arangodb::aql
} // namespace arangodb

#endif
