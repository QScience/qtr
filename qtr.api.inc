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
 * @param {mixed} $nid The item id
 * @param {mixed} $action The type of action
 * @param {mixed} $time Optional. The timestamp associated with 
 *   the action. Defaults, now()
 */
function qtr_write_action($uid, $nid, $aid, $time = REQUEST_TIME) {
    //check $uid
    if (!user_load($uid)) {
        return false;
    }
    //check $nid
    if (!node_load($nid)) {
        return false;
    }
    //check $aid
    if (!qtr_get_actiontype($aid)) {
        return false;
    }
    $query = db_select('qtr_actions', 'q');

    $result = $query->fields('q')
            ->condition('uid', $uid)
            ->condition('nid', $nid)
            ->condition('action_type', $aid)
            ->execute()
            ->fetchCol();
    if (!$result) {
        db_insert('qtr_actions')
                ->fields(array(
                    'uid' => $uid,
                    'nid' => $nid,
                    'action_type' => $aid,
                    'timestamp' => $time,
                ))
                ->execute();
    }
}

/**
 * Writes the values of the computation of QTR for 
 * 
 * @param {mixed} $table The table to insert the date
 * @param {mixed} $values the array of values to insert
 */
function qtr_write_values($table, $values) {
    // TODO
    if ($table == "qtr_reputation") {
        $sorted_values = array();
        for ($i = 0; $i < count($values); $i++) {
            $sorted_values += array($i => $values[$i]['reputation']);
        }
        arsort($sorted_values);
    }
    switch ($table) {
        case "qtr_reputation":
            $i = 0;
            foreach ($sorted_values as $id => $value) {
                db_query("INSERT INTO {qtr_reputation} (uid,rank,reputation) VALUES(:uid,:rank,:reputation) ON DUPLICATE KEY UPDATE rank=:rank, reputation=:reputation", array(
                    ":uid" => $id,
                    ":rank" => $i,
                    ":reputation" => $value
                ));
                $i++;
            }
            break;
        case "qtr_quality":
            foreach ($values as $id => $value) {
                db_query("INSERT INTO {qtr_quality} (nid,quality) VALUES(:nid,:quality) ON DUPLICATE KEY UPDATE quality=:quality", array(
                    ":nid" => $id,
                    ":quality" => $value['quality']
                ));
            }
            break;
    }
}

// GET ACTIONS

/**
 * Retrieves sets of actions from the QTR table
 *
 * 	FALSE if 
 */
function qtr_get_actions() {

//use join search from qtr_action and qtr_action_types to get action and weight values directly 
    $actions = db_query("SELECT a.uid, a.nid, at.action, a.timestamp, at.weight 
                   FROM {qtr_actions} AS a LEFT JOIN {qtr_action_types} AS at ON at.id=a.action_type 
                   INNER JOIN {node} AS n ON n.nid=a.nid where n.type IN (select item_type from {qtr_item_types})")->fetchAll();
    //foreach($actions)
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
    foreach ($types as $type) {
        $type+= array('reference_table' => 'node',);
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
    } else {
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
function qtr_add_actiontype($type = NULL, $weight = 0, $description = NULL) {
    if (is_null($type))
        return;
    else {
        db_insert('qtr_action_types')->fields(array('action' => $type, 'weight' => $weight, 'description' => $description))->execute();
    }
}

/**
 * 
 * get action types from qtr_action_types table
 */
function qtr_get_actiontype($aid = null) {
    $query = db_select('qtr_action_types', 'q');
    if ($aid == null) {
        $result = $query->fields('q')
                ->execute()
                ->fetchAll();
    } else {
        $result = $query->fields('q')
                ->condition('id', $aid)
                ->execute()
                ->fetchCol();
    }

    if (!empty($result)) {
        return $result;
    } else {
        return FALSE;
    }
}

/**
 * 
 * update action weight
 */
function qtr_update_actionweight($action, $weight) {
    db_query("UPDATE {qtr_action_types} SET weight = :weight WHERE action = :action", array('weight' => $weight, 'action' => $action));
}

/**
 * 
 * get top items
 * @param {mixed} $num The number of returned items
 * 
 */
function qtr_get_topquality($num) {
    $showlist = array();
    $count = 0;
    $types = array();
    foreach (node_type_get_types() as $type => $type_obj) {
        if (variable_get('popular_block_' . $type, 0)) {
            $types[] = $type;
        }
    }
    if (count($types) == 0) {
        return false;
    }

    $sql = 'SELECT n.nid,n.title FROM {qtr_quality} AS q INNER JOIN {node} as n ON q.nid=n.nid and n.type IN ' . $types . ' ORDER BY q.quality DESC LIMIT ' . $num;
    $result = db_query($sql)->fetchAll();
    return $result;
}

/**
 * Implements hook_exit().
 *
 * This is where actions are gathered on page accesses.
 */
function qtr_exit() {
    global $user;
    $uid = $user->uid;

    drupal_bootstrap(DRUPAL_BOOTSTRAP_VARIABLES);
    drupal_bootstrap(DRUPAL_BOOTSTRAP_SESSION);

    // For anonymous users unicode.inc will not have been loaded.
    include_once DRUPAL_ROOT . '/includes/unicode.inc';
    // Log this page access.
    $nid = '';
    $aid = '';
    if (arg(0) == 'node' && is_numeric(arg(1)) && arg(2) == NULL) {
        $nid = arg(1);
        $aid = 3;
    } else if (arg(0) == 'paper' && arg(1) == 'download' && is_numeric(arg(2))) {
        $nid = arg(2);
        $aid = 2;
    } else if (arg(0) == 'plus1' && arg(1) == 'vote' && arg(2) == 'node' && is_numeric(arg(3))) {
        $nid = arg(3);
        $aid = 1;
    }

    qtr_write_action($uid, $nid, $aid);
}
