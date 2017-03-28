{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <table><tr>
    <td>
      <h3> {g->text text="PHP Path Info Support"} </h3>

      <p class="giDescription">
        {g->text text="Testing if the server supports PHP Path Info."}
      </p>
    </td>
    <td style="float: right; vertical-align: top;">
      {if $TestResults.pathInfo == REWRITE_STATUS_OK}
        <h2 class="giSuccess"> {g->text text="Success"} </h2>
      {else}
        <h2 class="giError"> {g->text text="Error"} </h2>
      {/if}
    </td>
  {if $TestResults.pathInfo != $TestResults.truePathInfo}
  </tr><tr>
    <td colspan="2">
      <p class="giDescription giWarning">
        {g->text text="The current status may not be accurate, you have forced the test to pass."}
      </p>
    </td>
  {/if}
  {if $TestResults.pathInfo != REWRITE_STATUS_OK}
  </tr><tr>
    <td colspan="2">
      <div class="gbBlock">
        <h3> {g->text text="Test Path Info Manually"} </h3>

        <p class="giDescription">
          {g->text text="Gallery did not detect Path Info, please run this test yourself to verify."}
        </p>

        <table class="gbDataTable"><tr>
          <th> {g->text text="Force"} </th>
          <th> {g->text text="Test"} </th>
        </tr><tr>
          <td style="text-align: center;">
            <input type="checkbox" name="{g->formVar var="form[force][test]"}"/>
          </td>
          <td>
            <a href="{$TestResults.hrefTest}">{g->text text="PHP Path Info Test"}</a>
          </td>
        </tr></table>

        <p class="giDescription">
          {g->text text="If the test gives you a page that says PASS_PATH_INFO you are good to go."}
        </p>

      </div>
    </td>
  {/if}
  </tr></table>
</div>

