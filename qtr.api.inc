<?php
/**
 * @file
 * 
 * Api to write and access QTR data and the database
 */

// WRITE

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
	// TODO
}

/**
 * Writes the values of the computation of QTR for a specific
 * entity type
 * 
 * @param {mixed} $uid The user id from the users table
 * @param {mixed} $iid The item id
 * @param {mixed} $action The type of action
 * @param {mixed} $time Optional. The timestamp associated with 
 *   the action. Defaults, now()
 */
function qtr_write_values($table, $ids = array(), $values) {
	// TODO
}

// GET ACTIONS

/**
 * Retrieves sets of actions from the QTR table
 * 
 * @param $uid {mixed} Optional. If passed, returns the
 * 	actions only for the specified user id
 * @return $actions {Array|Boolean} The array of actions, or 
 * 	FALSE if 
 */
function qtr_get_actions() {
	// TODO
	return $actions;
}

function qtr_get_actions_by_user() {
	// TODO
	return $actions;
}

function qtr_get_actions_by_action_type($action_type = NULL) {
	// TODO
	return $actions;
}

function qtr_get_actions_by_entity_type($entity_type = NULL) {
	// TODO
	return $actions;
}


// GET VALUES

function qtr_get_values($entity_type, $id = NULL) {
	// TODO
	return $actions;
}



// QTR ADD NODE TYPE

/**
 * 
 * add item types to qtr_item_types table
 * @param {mixed} $types The node types
 */
function qtr_update_itemtype($types = array()) {
	//delete current item types
	$del = db_delete('qtr_item_types')->execute();
	//add new item types
	foreach($types as $type){
		$type+= array( 'reference_table' => 'node',);
		db_insert('qtr_item_types')->fields(array('item_type' => $type['item_type'], 'reference_table' => $type['reference_table']))->execute();	
	}
}
/**
 * 
 * get item types from qtr_item_types table
 */
function qtr_get_itemtype() {
	$query = db_select('qtr_item_types', 'q');
  	$result = $query->fields('q')
                  ->execute()
                  ->fetchAll();
    if (!empty($result)) {
    	return $result;
  	}
  	else {
    	return FALSE;
  	}
}

// QTR ADD ACTION TYPE


/**
 * 
 * Enter description here ...
 * @param {mixed} $type The action type to create
 * @param {mixed} $weight The action type to create
 * @param {mixed} $description The description associated to the action
 */
function qtr_add_actiontype($type = NULL, $weight=0, $description = NULL) {
	if (is_null($type)) return;
	else{
		db_insert('qtr_action_types')->fields(array('action' => $type, 'weight' => $weight, 'description'=>$description))->execute();
	}
}
/**
 * 
 * get action types from qtr_action_types table
 */
function qtr_get_actiontype() {
	$query = db_select('qtr_action_types', 'q');
  	$result = $query->fields('q')
                  ->execute()
                  ->fetchAll();
    if (!empty($result)) {
    	return $result;
  	}
  	else {
    	return FALSE;
  	}
}
/**
 * 
 * update action weight
 */
function qtr_update_actionweight($action, $weight) {
	 db_query("UPDATE {qtr_action_types} SET weight = :weight WHERE action = :action", array('weight'=>$weight,'action'=>$action));
}



// 
