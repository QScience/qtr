<?php
/**
 * @file
 * 
 * Api to write and access QTR data and the database
 */


/**
 * Writes a a new record in the QTR table
 * 
 * @param {mixed} $uid The user id from the users table
 * @param {mixed} $iid The item id
 * @param {mixed} $action The type of action
 * @param {mixed} $time Optional. The timestamp associated with 
 *   the action. Defaults, now()
 */
function qtr_write_action($uid, $iid, $action, $time = NULL) {
	
}

/**
 * Retrieves sets of actions from the QTR table
 * 
 * @param $uid {mixed} Optional. If passed, returns the
 * 	actions only for the specified user id
 * @return $actions {Array|Boolean} The array of actions, or 
 * 	FALSE if 
 */
function _qtr_get_actions($uid = NULL, $) {
	if (!is_null($uid)) {
		// Load actions for a specific user
	}
	else {
		// Load actions
	}
	
	return $actions;
}


/**
 * Retrieves sets of actions from the QTR table
 * 
 * @param $uid {mixed} Optional. If passed, returns the
 * 	actions only for the specified user id
 * @return $actions {Array|Boolean} The array of actions, or 
 * 	FALSE if 
 */
function _qtr_get_actions($uid = NULL, $) {
	if (!is_null($uid)) {
		// Load actions for a specific user
	}
	else {
		// Load actions
	}
	
	return $actions;
}

