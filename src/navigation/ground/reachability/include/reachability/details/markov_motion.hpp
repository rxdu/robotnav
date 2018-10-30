/* 
 * markov_motion.hpp
 * 
 * Created on: Oct 29, 2018 08:39
 * Description: Markov-chain longitudinal motion model
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#ifndef MARKOV_MOTION_HPP
#define MARKOV_MOTION_HPP

#include <cstdint>

namespace librav
{
// N: number of system states - i
// M: number of control inputs - alpha, beta
template <int32_t N, int32_t M>
class MarkovMotion : public MarkovChain<M * N>
{
  public:
    using Model = MarkovChain<M * N>;
    using State = typename MarkovChain<M * N>::State;
    using Transition = typename MarkovChain<M * N>::Transition;

  public:
    void SetupModel(State init)
    {
        
    }
};
} // namespace librav

#endif /* MARKOV_MOTION_HPP */
