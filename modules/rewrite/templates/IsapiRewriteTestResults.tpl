{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <table><tr>
    <td>
      <h3> {g->text text="IIS ISAPI_Rewrite Support"} </h3>

      <p class="giDescription">
        {g->text text="Testing if the server supports IIS ISAPI_Rewrite."}
      </p>
    </td>
    <td style="float: right; vertical-align: top;">
      {if $TestResults.isapiInfo == REWRITE_STATUS_OK}
        <h2 class="giSuccess"> {g->text text="Success"} </h2>
      {else}
        <h2 class="giError"> {g->text text="Error"} </h2>
      {/if}
    </td>
  {if $TestResults.isapiInfo != $TestResults.trueIsapiInfo}
  </tr><tr>
    <td colspan="2">
      <p class="giDescription giWarning">
        {g->text text="The current status may not be accurate, you have forced the test to pass."}
      </p>
    </td>
  {/if}
  {if $TestResults.isapiInfo != REWRITE_STATUS_OK}
  </tr><tr>
    <td colspan="2">
      <div class="gbBlock">
        <h3> {g->text text="How to setup ISAP Rewrite"} </h3>

        <p class="giDescription">
          {g->text text="In order to make the test pass you need to add the test rewrite rule in your httpd.ini:"}
        </p>

        <pre class="giDescription">{$TestResults.contents}</pre>

        <p class="giDescription"><b>{g->text text="If you add this at the bottom, please make sure that there's at least one empty line below the section"}</b></p>
      </div>

      <div class="gbBlock">
        <h3> {g->text text="Test ISAPI Rewrite Manually"} </h3>

        <p class="giDescription">
          {g->text text="Gallery did not detect ISAPI Rewrite, please run this test yourself to verify."}
        </p>

        <table class="gbDataTable"><tr>
          <th> {g->text text="Force"} </th>
          <th> {g->text text="Test"} </th>
        </tr><tr>
          <td style="text-align: center;">
            <input type="checkbox" name="{g->formVar var="form[force][test]"}"/>
          </td>
          <td>
            <a href="{$TestResults.hrefTest}">{g->text text="ISAPI Rewrite Test"}</a>
          </td>
        </tr></table>

        <p class="giDescription">
          {g->text text="If the test gives you a page that says PASS_ISAPI_REWRITE you are good to go."}
        </p>

      </div>
    </td>
  {/if}
  </tr><tr>
    <td>
      <h3> {g->text text="ISAPI_Rewrite httpd.ini file"} </h3>

      <p class="giDescription">
        {g->text text="Testing if Gallery can write to the httpd.ini file."}
      </p>
    </td>
    <td style="float: right; vertical-align: top;">
      {if $TestResults.httpdini == REWRITE_STATUS_OK}
        <h2 class="giSuccess"> {g->text text="Success"} </h2>
      {else}
        <h2 class="giError"> {g->text text="Error"} </h2>
      {/if}
    </td>
  {if $TestResults.httpdini != REWRITE_STATUS_OK}
  </tr><tr>
    <td colspan="2">
      <div class="gbBlock">
        {if $TestResults.httpdini == REWRITE_STATUS_HTTPDINI_MISSING}
        <h3> {g->text text="Please configure the correct location of ISAPI_Rewrite httpd.ini."} </h3>
        {/if}

        {if $TestResults.httpdini == REWRITE_STATUS_HTTPDINI_CANT_READ}
        <h3> {g->text text="Please make sure Gallery can read the httpd.ini file"} </h3>
        {/if}

        {if $TestResults.httpdini == REWRITE_STATUS_HTTPDINI_CANT_WRITE}
        <h3> {g->text text="Please make sure Gallery can write to the httpd.ini file"} </h3>
        {/if}
      </div>

    </td>
  {/if}
  </tr></table>
</div>
