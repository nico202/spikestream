/***************************************************************************
 *   SpikeStream Library                                                   *
 *   Copyright (C) 2007 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//-------------------------- PVM Messages ---------------------------------
/*! Defines the different messages that are used between tasks.
	This header is included in both SpikStream Application, SpikeStream 
	Archiver and SpikeStream Simulation. Message tags in PVM are integers.

	NOTE Any changes in this class require the recompilation of SpikeStream
	Application, SpikeStream Simulator and SpikeStream Archiver. */
//-------------------------------------------------------------------------

#ifndef PVMMESSAGES_H
#define PVMMESSAGES_H


/*! Confirms that archive task has started up.*/
#define ARCHIVE_TASK_STARTED_MSG 10


/*! Cancels request for firing neuron data.*/
#define CANCEL_FIRING_NEURON_DATA_MSG 20


/*! Cancels request of spike data.*/
#define CANCEL_SPIKE_DATA_MSG 30


/*! Message sent with error information.*/
#define ERROR_MSG 40


/*! Instructs task to quit enabling freeing of memory resources and other stuff.*/
#define EXIT_MSG 50


/*! Fires a list of specified neurons.*/
#define FIRE_SPECIFIED_NEURONS_MSG 60


/*! List of firing neurons. Differs from the spike list message because only 
	the neurons in the layer are included - not the to neurons.*/
#define FIRING_NEURON_LIST_MSG 70


/*! Message containing some information to help with debugging.*/
#define INFORMATION_MSG 80


/*! Instructs the task to create random firing patterns in its neurons.*/
#define INJECT_NOISE_MSG 90


/*! Instructs task to load the global parameters.*/
#define LOAD_GLOBAL_PARAMETERS_MSG 100


/*! Instructs task to load the neuron parameters from the database.*/
#define LOAD_NEURON_PARAMETERS_MSG 110


/*! Instructs task to load the noise parameters controlling the amount
	of noise in the neurons.*/
#define LOAD_NOISE_PARAMETERS_MSG 120


/*! Load communication information. Once all the tasks have written their task 
	ids to the database the main node instructs them to load up the neuron data.*/
#define LOAD_SIMULATION_DATA_MSG 130


/*! Instructs task to load synapse parameters from the database.*/
#define LOAD_SYNAPSE_PARAMETERS_MSG 140


/*! Instructs tasks to reload their weights from the database.*/
#define LOAD_WEIGHTS_MSG 150


/*! Sent when weights have been loaded successfully by a task.*/
#define LOAD_WEIGHTS_SUCCESS_MSG 160


/*! Data about the monitored state of the neuron.*/
#define MONITOR_NEURON_DATA_MSG 170


/*! Information about the data being monitored by the neuron.*/
#define MONITOR_NEURON_INFO_MSG 180


/*! Data about the monitored state of the synapse.*/
#define MONITOR_SYNAPSE_DATA_MSG 190


/*! Information about the data being monitored by the synapse.*/
#define MONITOR_SYNAPSE_INFO_MSG 200


/*! Request list of firing neurons to be sent to this task. */
#define REQUEST_FIRING_NEURON_DATA_MSG 210


/*! Request information about the variables that are available for monitoring 
	in a neuron.*/
#define REQUEST_MONITOR_NEURON_INFO_MSG 220


/*! Request information about the variables that are available for monitoring 
	in a synapse.*/
#define REQUEST_MONITOR_SYNAPSE_INFO_MSG 230


/*! Sent by network monitor to request spike list messages from a task processing 
	a neuron group.*/
#define REQUEST_SPIKE_DATA_MSG 240


/*! Instructs the tasks to write their current weights to the database.*/
#define SAVE_VIEW_WEIGHTS_MSG 250


/*! Instructs the tasks to write their current weights to the database.*/
#define SAVE_WEIGHTS_MSG 260


/*! Sets the minimum duration of each time step to slow the simulation down for 
	easier visualisation.*/
#define SET_MIN_TIMESTEP_DURATION_US_MSG 270


/*! Sets whether updates are event driven or whether all neurons and/or synapse 
	classes are updated at each timestep.*/
#define SET_UPDATE_MODE_MSG 280


/*! Sent after loading is complete.*/
#define SIMULATION_LOADING_COMPLETE_MSG 290


/*! Confirms that simulation task has started up.*/
#define SIMULATION_TASK_STARTED_MSG 300


/*! The list of spikes sent between tasks during simulation.*/
#define SPIKE_LIST_MSG 310


/*! Instructs the archiver to start archiving messages.*/
#define START_ARCHIVING_MSG 320


/*! Instructs the simulator to start sending monitoring data from a specified neuron.*/
#define START_MONITORING_NEURON_MSG 330


/*! Instructs the simulator to start sending monitoring data from a specified synapse.*/
#define START_MONITORING_SYNAPSE_MSG 340


/*! Start simulation. 
	Tasks are instructed to start simulating their neurons and propagating spike lists.*/
#define START_SIMULATION_MSG 350


/*! Step simulation.
	Advances the simulation one step and then stops it.*/
#define STEP_SIMULATION_MSG 360


/*! Instructs the archiver to stop archiving messages.*/
#define STOP_ARCHIVING_MSG 370


/*! Instructs the simulator to stop monitoring the specified neuron.*/
#define STOP_MONITORING_NEURON_MSG 380


/*! Instructs the simulator to stop monitoring the specified neuron.*/
#define STOP_MONITORING_SYNAPSE_MSG 390


/*! Stop simulation.
	Tasks are instructed to stop simulating neurons and propagating spike lists.*/
#define STOP_SIMULATION_MSG 400


/*! Sent by a task when it is just about to exit.*/
#define TASK_EXITED_MSG 410


/*! Message sent as a test*/
#define TEST_MSG 420


/*! Confirms that weights have been successfully saved.*/
#define VIEW_WEIGHTS_SAVE_SUCCESS_MSG 430


/*! Confirms that weights have been successfully saved.*/
#define WEIGHT_SAVE_SUCCESS_MSG 440



#endif//PVMMESSAGES_H

