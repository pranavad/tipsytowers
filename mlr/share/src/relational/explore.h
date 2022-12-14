#if 0

#ifndef TL__RULE_EXPLORER
#define TL__RULE_EXPLORER

#include "robotManipulationSymbols.h"
#include "robotManipulationSimulator.h"
#include "learn.h"
#include "ruleLearner_ground.h"
#include "plan.h"
#include "prada.h"


// ---------------------------------------------------
//       FLAGS

#define MOVE_TYPE__EXPLOIT 1
#define MOVE_TYPE__EXPLORE_PLANNED 2
#define MOVE_TYPE__EXPLORE_DIRECT 3



// ---------------------------------------------------
//       EXPLORATION - PARAMETERS

// General
#define RULE_CONFIDENCE_THRESHOLD 2.0
#define MIN_PATTERN_ACTION_REPEATS 2
#define BAD_PATTERN_MIN_LENGTH 2
#define ENTROPY_RELATED_OBJECTS_DEPTH 1   // 1 am besten bei box-clearance

// Planned explore
#define PLANNED_EXPLORE__NUMBER_PRADA_SAMPLES 300
#define PLANNED_EXPLORE__HORIZON 3
#define PLANNED_EXPLORE__THRESHOLD 0.2





extern RobotManipulationSimulator sim;

namespace PRADA {
  
  
struct RelationalStateGraph;
  


  
  
class RuleExplorer {
  public:
    
  enum BehaviorType {epsilon_greedy, ecube, rmax};
  enum DensityEstimationType {density_simple, density_entropy};
  enum RepresentationType {relational, factored, flat};
  
  
  // Greedy
  double greedy_epsilon_start;
  
    
  RepresentationType representation;
  
  // Rules
  arr rules__confidences; // offen fuer bearbeitung
  RuleSet confident_ground_rules;
  
  // Rule learning
  double complexity_penalty_coeff;
  double p_lower_bound__noise_outcome;
  double p_lower_bound__noise_outcome_in_default_rule;
  
  // Actions
  LitL modeledActions;
  LitL fixedActions;
  boolA learners_uptodate;
  uintA actions__covering_rules;
  mlr::Array< uintA > actions__covering_rules__all;
  arr actions__confidences;
  arr actions__dissimilarity_to_previous_experiences__local;  // TODO NEU berechnet Konfidenzen entsprechend der Zustaende in den Erfahrungen (ohne auf Regeln zu schauen)
  boolA action__has_interesting_argument;
  boolA action__is_known;
  
  LitL possibleGroundActions;
  
  // SymbolicState measures
  SymbolicState current_state;
  bool current_state_is_known;
  bool current_state_is_known_partially;
  
  // Experiences
  mlr::Array< uintA > experiences_per_modeledAction;
  StateTransitionL all_experiences;
  arr experience_weights;
  boolA is_major_experience;
  mlr::Array<SymbolicState*> visited_pre_states;
  LitL visited_actions;
  
  // Relevant objects
  uintA reward_constants;
  uintA reward_constants_neighbors;
  
  // Planned exploration
  Reward* planned_explore_reward;
  uint planned_explore_reward__timestamp;
  LitL very_old_exploration_rewards;
  LitL very_very_old_exploration_rewards;
  
  // only for statistics
  uintA moves_explore_direct;
  uintA moves_explore_planned;
  uintA moves_exploit;
  uintA moves;
  
  // auxiliary structures
  LitL last_exploit_plan;
  
  // only for presentation
  mlr::String message;
  
  virtual uint action_to_learner_id(Literal* action) = 0;
  
  
  
  // --------------------------------------------------------------------------------------------
  
  explicit RuleExplorer(RepresentationType representation,
                        double complexity_penalty_coeff, double p_lower_bound__noise_outcome, double p_lower_bound__noise_outcome_in_default_rule);
  ~RuleExplorer();
    
  // Confidence calculations (--> true state novelty calculation / job is done here)
  double calcRuleConfidence(uint rule_id);
  void calcExploreWeights(arr& explore_weights, Literal* taboo_action = NULL);
  
  // Propagate confidence calculations
  virtual void updateActionAndStateMeasures();
  
  // Access state informaction
  void getKnownStates(SymbolicStateL& known_states); // aus rules, all_ground_actions und visited_states zusammengesetzt
  bool stateIsKnown(const SymbolicState& state, const LitL& ground_actions);
  bool stateIsKnown_partially(const SymbolicState& state, const LitL& ground_actions);
  virtual bool actionIsKnown(const SymbolicState& state, Literal* action);
  virtual uint getNumberOfCoveredExperiences(uint rule_id) = 0;
  
  // Decision-making
  Literal* getExploitAction(LitL& exploit_plan, PRADA_Planner* prada, const SymbolicState& state);
  Literal* getExploreAction_planned__contexts(LitL& explore_plan, PRADA_Planner* prada, const SymbolicState& state);
  Literal* getExploreAction_planned(LitL& explore_plan, PRADA_Planner* prada, const SymbolicState& state, uint type = 1);
  Literal* getExploreAction_direct(PRADA_Planner* prada, const SymbolicState& state, Literal* taboo_action = NULL);
  
  // Methods for EXTERNAL use
  virtual Literal* decideAction(const SymbolicState& state, NID_Planner* planner, uint behavior_type, bool use_known_state_partial);
  void addObservation__helper(SymbolicState* state_pre, Literal* action, SymbolicState* state_post);
  virtual void addObservation(SymbolicState* state_pre, Literal* action, SymbolicState* state_post) = 0;
  void addObservations(const StateTransitionL& trial);
  
  virtual void updateRules(bool always_re_learning = true) = 0;
  virtual void updateLogicEngineConstants();  // das hier vielleicht doch nicht, oder?
  virtual void reset();
  virtual const RuleSet& getRules() = 0;
  virtual void get_nonDefaultRules_for_last_experience(uintA& rule_ids) = 0;
};
  




class AbstractRuleExplorer : public RuleExplorer {
public:
  RuleSetContainer rulesC;
  mlr::Array<RuleLearner*> learners;
  
  arr rule_experiences_entropies;
  
  // For more sophisticated density estimators
  RelationalStateGraph* current_graph;
  mlr::Array< RelationalStateGraph* > graphs;
  
  uint density_estimation_type;
  
  public:
    AbstractRuleExplorer(double complexity_penalty_coeff, double p_lower_bound__noise_outcome, double p_lower_bound__noise_outcome_in_default_rule,
                          RuleSet& fixed_rules_for_fixed_actions, uint density_estimation_type);
    ~AbstractRuleExplorer();
    virtual void init_rules();
    uint action_to_learner_id(Literal* action);
    void updateRules(bool always_re_learning = true);
    
    void set_p_lower_bounds(double p_lower_bound__noise_outcome, double p_lower_bound__noise_outcome_in_default_rule);
    
    // Gehoert eigentlich in die Superklasse:  Problem rulesC muesste vereinheitlicht werden...
    const RuleSet& getRules() {return rulesC.rules;}
    void get_nonDefaultRules_for_last_experience(uintA& rule_ids) {rule_ids = rulesC.nonDefaultRules_per_experience.last();}
    uint getNumberOfCoveredExperiences(uint rule_id) {return rulesC.experiences_per_rule(rule_id).N;}
    void addObservation(SymbolicState* state_pre, Literal* action, SymbolicState* state_post);
    void setFixedRulesForAction(RuleSet& rules_for_action);
    double calcRuleConfidence(uint rule_id);
    void updateActionAndStateMeasures();
};


class FactoredRuleExplorer : public RuleExplorer {
  RuleSetContainer_ground rulesC;
  mlr::Array<RuleLearner_ground*> learners;
  
  public:
    FactoredRuleExplorer(double complexity_penalty_coeff, double p_lower_bound__noise_outcome, double p_lower_bound__noise_outcome_in_default_rule,
                         RuleSet& fixed_rules_for_fixed_actions);
    ~FactoredRuleExplorer();
    virtual void init_rules();
    uint action_to_learner_id(Literal* action);
    void updateRules(bool always_re_learning = true);
    void updateLogicEngineConstants();
    void reset();
    
    // Gehoert eigentlich in die Superklasse:  Problem rulesC muesste vereinheitlicht werden...Z
    const RuleSet& getRules() {return rulesC.rules;}
    void get_nonDefaultRules_for_last_experience(uintA& rule_ids) {rule_ids = rulesC.nonDefaultRules_per_experience.last();}
    uint getNumberOfCoveredExperiences(uint rule_id) {return rulesC.experiences_per_rule(rule_id).N;}
    void addObservation(SymbolicState* state_pre, Literal* action, SymbolicState* state_post);
};



class FlatExplorer : public RuleExplorer {
  mlr::Array< RuleSet > rules_hierarchy;  // dim 0: actions;  dim 1: states
  mlr::Array< mlr::Array< uintA > > experiences_per_rule__hierarchy;
  mlr::Array< uintA > rule_confidences__hierarchy;
  // --> default rule:  hier ganz hinten
  
  // redundant rule-container
  RuleSet rules_flat;
  mlr::Array< uintA > experiences_per_rule__flat;
  // --> default rule:  hier ganz vorne
  
  RuleSet fixed_rules_memory;
  
  public:
    FlatExplorer(RuleSet& fixed_rules_for_fixed_actions);
    ~FlatExplorer();
    void init_rules();
    uint action_to_learner_id(Literal* action);
    int action_state_to_flat_id(Literal* action, SymbolicState* state);
    void updateRules(bool always_re_learning = true);
    void updateLogicEngineConstants();
    void reset();
    void addObservation(SymbolicState* state_pre, Literal* action, SymbolicState* state_post);
    
    // Gehoert eigentlich in die Superklasse:  Problem rulesC muesste vereinheitlicht werden...Z
    const RuleSet& getRules() {return rules_flat;}
    void get_nonDefaultRules_for_last_experience(uintA& rule_ids);
    uint getNumberOfCoveredExperiences(uint rule_id) {return experiences_per_rule__flat(rule_id).N;}
};




#if 1
struct RelationalStateGraph {
  
  const SymbolicState state;
  uintA constants;
  
  LitL lits_zeroary;
  mlr::Array< LitL > lits_unary;
  mlr::Array< LitL > lits_binary; // 2-dim
  boolA lits_binary_matrix;  // adjacency_matrix(a,b)  iff  a-->b  iff predicate(a,b)
  FuncVL fvs_zeroary;
  mlr::Array< FuncVL > fvs_unary;
  
  RelationalStateGraph(const SymbolicState& state);
  ~RelationalStateGraph();
  
  void getRelatedConstants(uintA& related_constants, uint obj, uint depth) const;
  void getRelatedConstants(uintA& related_constants, const uintA& objects, uint depth) const;
  void getDirectNeighbors(uintA& neighbors, uint obj) const;
  
  RelationalStateGraph* getSubgraph(const uintA& objects) const;
  
  void writeNice(ostream& os = cout) const;
  
  
  
  // distance in [0,1]
  static double distance(const RelationalStateGraph& g1, const Literal& a1,
                         const RelationalStateGraph& g2, const Literal& a2);
  
  static double entropy(const LitL& actions, const mlr::Array< RelationalStateGraph* >& graphs);
  
//   RelationalStateGraph* createGraph(const Literal& action, const SymbolicState& state, const Rule& rule, uint depth);
  static RelationalStateGraph* createSubgraph(const Literal& action, const RelationalStateGraph& full_graph, const Rule& rule, uint depth);
  
  static double getMinDistance(const RelationalStateGraph& graph, const Literal& action,
                               const mlr::Array< RelationalStateGraph* > other_graphs, const LitL& other_actions);
  
};
#endif


}

#endif // TL__RULE_EXPLORER

#endif
