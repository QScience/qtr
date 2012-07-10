<?php
/**
 * @file
 * Private QTR settings and functions.
 */

function _qtr_save($agent, $item, $agent_file, $item_file) {
  $ofs_AG = fopen($agent_file, "w");
  $ofs_IT = fopen($item_file, "w");
  for ($i = 0; $i < count($agent); $i++) {
    fwrite($ofs_AG, "$i\t" . $agent[$i]['reputation'] . "\n");
  }
  for ($a = 0; $a < count($item); $a++) {
    fwrite($ofs_IT, "$a\t" . $item[$a]['quality'] . "\n");
  }
  fclose($ofs_AG);
  fclose($ofs_IT);
}

function _qtr_init(&$agent, &$item, $agent_num, $item_num) {

  $agent = array_fill(0, $agent_num, array(
    'reputation' => 0.0, // reputation value
    'r_tmp' => 0.0,
    'collection' => array(), // ID of items the user has interacted with
    'c_time' => array(), // time when user interacted with an item
    'c_weight' => array(), // weight associated to the interaction
    'neighbor' => array(), // ID of trusted fellows of the users
    't_exp' => array() // amount of trust
  ));

  $item = array_fill(0, $item_num, array(
    'quality' => 0.0, // quality value
    'q_tmp' => 0.0,
    'reader' => array(), // ID of users who interacted with item
    'r_time' => array(), // time when an user interacted with item
    'r_weight' => array() // weight associated to the interaction
  ));

}

function _qtr_read(&$agent, &$item, $user_item_file, $trust) {
  // $trust is for explicit trust.

  /* reading input file for user-item interactions */
  /* FORMAT: userID \t itemID \t interaction_type \t interaction_time
   * (eventually, \t kind of object) */
  // N.B.: the time units are decided here; set variables in lines 27 and 38
  // accordingly
  $ifs = file($user_item_file);
  foreach ($ifs as $line) {
    $words = explode("\t", $line);
    $i = (int) $words[0];
    $a = (int) $words[1];
    $rating = (string) $words[2];
    $time = (int) $words[3];
    if (!in_array($a, $agent[$i]['collection'])) { // this is to avoid duplicates; it is possible to remove this control
      $item[$a]['reader'][] = $i; $agent[$i]['collection'][] = $a;
      $item[$a]['r_time'][] = $time; $agent[$i]['c_time'][] = $time;
      // follows the way weights are expressed.
      switch ($rating) {
      case "U":
        $rw = variable_get('qtr_w_upload', 1.0);
        break;
      case "D":
        $rw = variable_get('qtr_w_download', 0.1);
        break;
      case "V":
        $rw = variable_get('qtr_w_view', 0.05);
        break;
      }
      $item[$a]['r_weight'][] = $rw;
      $agent[$i]['c_weight'][] = $rw;
    }
  }
  /* now if $trust is not NULL, reading input file for trust relationships */
  /* FORMAT: userID \t ID_of_user_trusted_by_i \t amount_of_trust */
  if ($trust !== NULL) {
    $ifs_ET = file($trust);
    foreach ($ifs_ET as $line) {
      $words = explode("\t", $line);
      $i = (int) $words[0];
      $j = (int) $words[1];
      $tt = (double) $words[2];
      $agent[$i]['neighbor'][] = $j;
      $agent[$i]['t_exp'][$tt];
    }
  }
}

function _qtr_decay($tau) {
  $tau0 = variable_get('qtr_tau0', 50);
  switch (variable_get('qtr_decay', 0)) {
  case 1:
    return pow(1. + ((double)$tau) / $tau0, -1);
  case 2:
    return exp(-((double)$tau) / $tau0);
  case 3:
    return $tau <= $tau0 ? 1.0 : 0.0;
  default:
    return 1.0;
  }
}

function _qtr_calculate($agent, $item, $trust, $start) {
  drupal_set_message(t('Starting QTR'));

  // initializing the error an the average values; computing average trust //
  $err = count($agent) * count($item); $avg_q = 0; $avg_r = 0; $avg_t = 0; $nn = 0;
  if ($trust !== NULL) {
    for ($i = 0; $i < count($agent); $i++) {
      for ($j = 0; $j < count($agent[$i]['neighbor']); $j++) {
        $avg_t += $agent[$i]['t_exp'][$j];
        $nn++;
      }
    }
    $avg_t /= $nn;
  }

  // setting the initial values of reputation and quality //
  for ($a = 0; $a < count($item); $a++) {
    $item[$a]['q_tmp'] = 1. / sqrt(count($item));
  }
  for ($i = 0; $i < count($agent); $i++) {
    $agent[$i]['r_tmp'] = 1. / sqrt(count($agent));
  }

  // iterations //
  $flag = 0;
  while ($err > variable_get('qtr_delta', 0.00000000001)) {
    $err = 0;
    $norm = 0;
    for ($a = 0; $a < count($item); $a++) {
      $item[$a]['quality'] = 0;
      for ($r = 0; $r < count($item[$a]['reader']); $r++) {
        $item[$a]['quality'] += $item[$a]['r_weight'][$r] * ($agent[$item[$a]['reader'][$r]]['r_tmp'] - variable_get('qtr_resc_r', 0) * $avg_r) * _qtr_decay($start - $item[$a]['r_time'][$r]) /
          pow(count($item[$a]['reader']), variable_get('qtr_renorm_q', 0));
      }
      $norm += pow($item[$a]['quality'], 2);
    }
    // normalization of newly obtained qualities and updating of error
    for ($a = 0; $a < count($item); $a++) {
      $item[$a]['quality'] /= sqrt($norm);
      $err += abs($item[$a]['quality'] - $item[$a]['q_tmp']);
    }
    // cycle on users to determine reputations
    $norm = 0;
    for ($i = 0; $i < count($agent); $i++) {
      $agent[$i]['reputation'] = 0;
      for ($a = 0; $a < count($agent[$i]['collection']); $a++) {
        $agent[$i]['reputation'] += $agent[$i]['c_weight'][$a] * ($item[$agent[$i]['collection'][$a]]['q_tmp'] - variable_get('qtr_resc_q', 0) * $avg_q) *
          _qtr_decay($start - $agent[$i]['c_time'][$a]) / pow(count($agent[$i]['collection']), variable_get('qtr_renorm_r', 0));
      }
      if ($trust !== NULL) {
        for ($j = 0; $j < count($agent[$i]['neighbor']); $j++) {
          $agent[$i]['reputation'] += ($agent[$agent[$i]['neighbor'][$j]]['r_tmp'] - variable_get('qtr_resc_t', 0) * $avg_r) * ($agent[$i]['t_exp'][$j] - variable_get('qtr_resc_t', 0) * $avg_t) /
            pow($agent[$i]['reputation'], 2);
        }
      }
      $norm += pow($agent[$i]['reputation'], 2);
    }
    for ($i = 0; $i < count($agent); $i++) {
      $agent[$i]['reputation'] /= sqrt($norm);
      $err += abs($agent[$i]['reputation'] - $agent[$i]['r_tmp']);
    }

    // updating scores and averages //
    $avg_q = 0; $avg_r = 0;
    for ($a = 0; $a < count($item); $a++) {
      $item[$a]['q_tmp'] = $item[$a]['quality'];
      $avg_q += $item[$a]['quality'] / count($item);
    }
    for ($i = 0; $i < count($agent); $i++) {
      $agent[$i]['r_tmp'] = $agent[$i]['reputation'];
      $avg_r = $agent[$i]['reputation'] / count($agent);
    }
    // displaying the current iteration step
    $flag++;
    drupal_set_message($flag . " " . $err);
  }
}

