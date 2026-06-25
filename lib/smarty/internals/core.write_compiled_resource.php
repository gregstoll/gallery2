<?php
/**
 * Smarty plugin
 * @package Smarty
 * @subpackage plugins
 */

/**
 * write the compiled resource
 *
 * @param string $compile_path
 * @param string $compiled_content
 * @return true
 */
function smarty_core_write_compiled_resource($params, &$smarty)
{
    // PHP caches stat() results; a probe earlier this request (e.g. in
    // _initCompiledTemplateDir) can leave is_writable()/is_dir() returning a
    // stale value for compile_dir. Flush it before trusting the checks below.
    clearstatcache();

    if(!@is_writable($smarty->compile_dir)) {
        // compile_dir not writable, see if it exists
        if(!@is_dir($smarty->compile_dir)) {
            // Not in the (possibly stale) cache as a directory: try to create
            // it for real, then re-test against disk before giving up.
            $_params = array('dir' => $smarty->compile_dir);
            require_once(SMARTY_CORE_DIR . 'core.create_dir_structure.php');
            smarty_core_create_dir_structure($_params, $smarty);
            clearstatcache();
            if(!@is_dir($smarty->compile_dir)) {
                $smarty->trigger_error('the $compile_dir \'' . $smarty->compile_dir . '\' does not exist, or is not a directory.', E_USER_ERROR);
                return false;
            }
        }
        // It is a directory; re-test writability against disk (not the cache).
        if(!@is_writable($smarty->compile_dir)) {
            $smarty->trigger_error('unable to write to $compile_dir \'' . realpath($smarty->compile_dir) . '\'. Be sure $compile_dir is writable by the web server user.', E_USER_ERROR);
            return false;
        }
    }

    $_params = array('filename' => $params['compile_path'], 'contents' => $params['compiled_content'], 'create_dirs' => true);
    require_once(SMARTY_CORE_DIR . 'core.write_file.php');
    smarty_core_write_file($_params, $smarty);
    return true;
}

/* vim: set expandtab: */

?>
