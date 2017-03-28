{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <table><tr>
    <td>
      <h3> {g->text text="Apache mod_rewrite"} </h3>

      {capture name=mod_rewrite_anchor}
      <a href="http://httpd.apache.org/docs/mod/mod_rewrite.html">mod_rewrite</a>
      {/capture}
      <p class="giDescription">
        {g->text text="Testing if %s is supported by the server." arg1=$smarty.capture.mod_rewrite_anchor}
      </p>
    </td>
    <td style="float: right; vertical-align: top;">
      {if $TestResults.gallery.modRewrite == REWRITE_STATUS_OK}
        <h3 class="giSuccess"> {g->text text="Success"} </h3>
      {else}
        <h3 class="giWarning"> {g->text text="Warning"} </h3>
      {/if}
    </td>
  {if $TestResults.gallery.modRewrite != $TestResults.gallery.trueModRewrite}
  </tr><tr>
    <td colspan="2">
      <p class="giDescription giWarning">
        {g->text text="The current status may not be accurate, you have forced the test to pass."}
      </p>
    </td>
  {/if}
  {if $TestResults.gallery.modRewrite != REWRITE_STATUS_OK}
  </tr><tr>
    <td colspan="2">
      {if $TestResults.gallery.modRewrite != REWRITE_STATUS_MULTISITE}
        {if $TestResults.gallery.modRewrite == REWRITE_STATUS_APACHE_UNABLE_TO_TEST}
        <div class="gbBlock">
          <h3> {g->text text="Custom Gallery directory test setup"} </h3>

          <p class="giDescription">
            {g->text text="Gallery tries to test mod_rewrite in action. For this to work you need to edit each of these two files accordingly:"}
          </p>

          <p class="giDescription">
            <b>{$TestResults.gallery.customFile1}</b><br/>
            {g->text text="Line 6:"} {$TestResults.gallery.customLine1}
          </p>

          <p class="giDescription">
            <b>{$TestResults.gallery.customFile2}</b><br/>
            {g->text text="Line 6:"} {$TestResults.gallery.customLine2}
          </p>
        </div>
        {/if}

      <div class="gbBlock">
        <h3> {g->text text="Test mod_rewrite manually"} </h3>

        <p class="giDescription">
          {g->text text="For whatever reason, Gallery did not detect a working mod_rewrite setup. If you are confident that mod_rewrite does work you may override the automatic detection. Please, run these two tests to see for yourself."}
        </p>

        <table class="gbDataTable"><tr>
          <th> {g->text text="Works"} </th>
          <th> {g->text text="Test"} </th>
        </tr><tr>
          <td style="text-align: center;">
            <input type="checkbox" name="{g->formVar var="form[force][test1]"}"/>
          </td>
          <td>
            <a href="{$TestResults.href.test1}">{g->text text="mod_rewrite configuration 1 (with global Options +FollowSymlinks)"}</a>
          </td>
        </tr><tr>
          <td style="text-align: center;">
            <input type="checkbox" name="{g->formVar var="form[force][test2]"}"/>
          </td>
          <td>
            <a href="{$TestResults.href.test2}">{g->text text="mod_rewrite configuration 2 (with local Options +FollowSymlinks)"}</a>
          </td>
        </tr></table>

        <p class="giDescription">
          {g->text text="If one of the two tests gives you a page with the text PASS_REWRITE you are good to go."}
        </p>

      </div>
      {else}
      <div class="gbBlock">
        <h3> {g->text text="Apache mod_rewrite and Gallery multisite"} </h3>

        <p class="giDescription">
          {g->text text="Gallery tries to test mod_rewrite in action. This does not work with multisite since Gallery lacks the complete codebase."}
        </p>

        <table class="gbDataTable"><tr>
          <th> {g->text text="Force"} </th>
          <th> {g->text text="Test"} </th>
        </tr><tr>
          <td style="text-align: center;">
            <input type="checkbox" name="{g->formVar var="form[force][test1]"}"/>
          </td>
          <td>
            {g->text text="mod_rewrite configuration 1 (with global Options +FollowSymlinks)"}
          </td>
        </tr><tr>
          <td style="text-align: center;">
            <input type="checkbox" name="{g->formVar var="form[force][test2]"}"/>
          </td>
          <td>
            {g->text text="mod_rewrite configuration 2 (with local Options +FollowSymlinks)"}
          </td>
        </tr></table>

      </div>
      {/if}

    </td>
  {/if}
  </tr><tr>
    <td>
      <h3> {g->text text="Gallery .htaccess file"} </h3>

      <p class="giDescription">
        {g->text text="Testing if Gallery can write to the .htaccess file."}
      </p>
    </td>
    <td style="float: right; vertical-align: top;">
      {if $TestResults.gallery.htaccess == REWRITE_STATUS_OK}
        <h2 class="giSuccess"> {g->text text="Success"} </h2>
      {else}
        <h2 class="giError"> {g->text text="Error"} </h2>
      {/if}
    </td>
  {if $TestResults.gallery.htaccess != REWRITE_STATUS_OK}
  </tr><tr>
    <td colspan="2">
      <div class="gbBlock">
        {if $TestResults.gallery.htaccess == REWRITE_STATUS_HTACCESS_MISSING}
        <h3> {g->text text="Please create a file in your Gallery directory named .htaccess"} </h3>

        <pre class="giDescription">touch {$TestResults.gallery.htaccessPath}<br/>chmod 666 {$TestResults.gallery.htaccessPath}</pre>
        {/if}

        {if $TestResults.gallery.htaccess == REWRITE_STATUS_HTACCESS_CANT_READ}
        <h3> {g->text text="Please make sure Gallery can read the existing .htaccess file"} </h3>

        <pre class="giDescription">chmod 666 {$TestResults.gallery.htaccessPath}</pre>
        {/if}

        {if $TestResults.gallery.htaccess == REWRITE_STATUS_HTACCESS_CANT_WRITE}
        <h3> {g->text text="Please make sure Gallery can write to the existing .htaccess file"} </h3>

        <pre class="giDescription">chmod 666 {$TestResults.gallery.htaccessPath}</pre>
        {/if}
      </div>

    </td>
  {/if}
  {if isset($TestResults.embedded)}
  </tr><tr>
    <td>
      <h3> {g->text text="Embedded .htaccess file"} </h3>

      <p class="giDescription">
        {g->text text="Testing if Gallery can write to the embedded .htaccess file."}
      </p>
    </td>
    <td style="float: right; vertical-align: top;">
      {if $TestResults.embedded.htaccess == REWRITE_STATUS_OK}
        <h2 class="giSuccess"> {g->text text="Success"} </h2>
      {else}
        <h2 class="giError"> {g->text text="Error"} </h2>
      {/if}
    </td>
  {if $TestResults.embedded.htaccess != REWRITE_STATUS_OK}
  </tr><tr>
    {if $TestResults.embedded.htaccessPath == '/.htaccess'}
    <td>
      <div class="gbBlock">
        <p class="giDescription">
          {g->text text="Please configure the embedded htaccess path."}
        </p>
      </div>
    </td>
    {else}
    <td colspan="2">
      <div class="gbBlock">
        {if $TestResults.embedded.htaccess == REWRITE_STATUS_HTACCESS_MISSING}
        <h3> {g->text text="Please create a file in your Gallery directory named .htaccess"} </h3>

        <pre class="giDescription">touch {$TestResults.embedded.htaccessPath}<br/>chmod 666 {$TestResults.embedded.htaccessPath}</pre>
        {/if}

        {if $TestResults.embedded.htaccess == REWRITE_STATUS_HTACCESS_CANT_READ}
        <h3> {g->text text="Please make sure Gallery can read the existing .htaccess file"} </h3>

        <pre class="giDescription">chmod 666 {$TestResults.embedded.htaccessPath}</pre>
        {/if}

        {if $TestResults.embedded.htaccess == REWRITE_STATUS_HTACCESS_CANT_WRITE}
        <h3> {g->text text="Please make sure Gallery can write to the existing .htaccess file"} </h3>

        <pre class="giDescription">chmod 666 {$TestResults.embedded.htaccessPath}</pre>
        {/if}
      </div>

    </td>
    {/if}
  {/if}
  {/if}
  </tr></table>
</div>
